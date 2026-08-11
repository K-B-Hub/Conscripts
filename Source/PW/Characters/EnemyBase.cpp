// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/EnemyBase.h"
#include "Characters/AllyCharacterBase.h"
#include "AI/AIController/EnemyAIController.h"
#include "AI/UtilityAIComponent.h"
#include "GameMode/BattleGameMode.h"
#include "Components/WidgetComponent.h"
#include "DrawDebugHelpers.h"
#include "DataAsset/UpgradeLibrary.h"
#include "DataAsset/FixedUpgradeTableData.h"
#include "GameInstance/PWGameInstance.h"
#include "Object/Skill/SkillBase.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	utilityAI = CreateDefaultSubobject<UUtilityAIComponent>(TEXT("UtilityAI"));
}

void AEnemyBase::GrantLevelUpUpgrade()
{
	//고정 강화 레벨이면 지정된 강화를 그대로 습득, 아군의 단일 카드 제시와 같은 규칙
	const ELevelUpUpgradeKind kind = UUpgradeLibrary::ClassifyLevelUpUpgrade(level);
	if (kind == ELevelUpUpgradeKind::ClassFixed)
	{
		if (fixedUpgradeTable)
		{
			AcquireUpgrade(fixedUpgradeTable->GetFixedUpgrade(level));
		}
		return;
	}

	//공용 풀은 GameInstance, 직업 풀은 자기 것
	const UPWGameInstance* gameInstance = GetWorld() ? GetWorld()->GetGameInstance<UPWGameInstance>() : nullptr;
	UUpgradeTableData* commonTable = gameInstance ? gameInstance->GetCommonUpgradeTable() : nullptr;

	//고급 랜덤 레벨은 하급 풀 제외
	const EUpgradeGrade minGrade = (kind == ELevelUpUpgradeKind::HighRandom) ? EUpgradeGrade::Mid : EUpgradeGrade::Low;

	//적은 선택지 없이 랜덤 1개를 자동 습득
	TArray<TSubclassOf<USkillBase>> choices = UUpgradeLibrary::BuildChoices(this, classUpgradeTable, commonTable, 1, minGrade);
	if (choices.Num() > 0)
	{
		AcquireUpgrade(choices[0]);
	}
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
	//상대좌표의 기준점 — 배치 위치 캡처
	spawnLocation = GetActorLocation();
	//원위치(상대좌표 0)를 순찰 지점에 추가 — 1개만 입력해도 원위치와 왕복
	patrolPoints.AddUnique(FVector::ZeroVector);

	//시야 판정 전 기본 숨김, 첫 Tick에서 갱신
	SetActorHiddenInGame(true);
	if (healthWidgetComponent)
	{
		healthWidgetComponent->SetVisibility(false);
	}
}

FVector AEnemyBase::GetCurrentPatrolWorldLocation() const
{
	//호출 측에서 CanPatrol로 가드하는 전제 — 방어적으로 한 번 더 체크
	if (!patrolPoints.IsValidIndex(patrolIndex)) return spawnLocation;
	return spawnLocation + patrolPoints[patrolIndex];
}

void AEnemyBase::AdvancePatrolIndex()
{
	const int32 step = bPatrolReverse ? -1 : 1;
	int32 next = patrolIndex + step;

	//끝점 도달 — 방향 뒤집어 되돌아감
	if (!patrolPoints.IsValidIndex(next))
	{
		bPatrolReverse = !bPatrolReverse;
		next = patrolIndex - step;
	}

	//지점이 1개뿐이면 뒤집어도 범위 밖 → 인덱스 유지(그 한 점을 계속 순회)
	if (patrolPoints.IsValidIndex(next))
	{
		patrolIndex = next;
	}
}

void AEnemyBase::InitTurn()
{
	Super::InitTurn();

	if (AEnemyAIController* aic = Cast<AEnemyAIController>(GetController()))
	{
		aic->OnEnemyTurnStart();
	}
}

void AEnemyBase::UpdatePlayerVisibility()
{
	//한번 관측된 적은 시야 밖으로 나가도 영구 가시
	if (bVisibleToPlayers) return;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	//생존 아군 하나라도 시야 반경(XY) 안이면 가시
	bool bVisible = false;
	const FVector myLoc = GetActorLocation();
	for (AAllyCharacterBase* ally : gm->GetAllies())
	{
		if (!IsValid(ally) || ally->IsDead()) continue;
		if (FVector::Dist2D(ally->GetActorLocation(), myLoc) <= ally->GetSight() * 100.f)
		{
			bVisible = true;
			break;
		}
	}

	//변화 시에만 숨김 상태 갱신
	if (bVisible == bVisibleToPlayers) return;
	bVisibleToPlayers = bVisible;
	SetActorHiddenInGame(!bVisible);
	if (healthWidgetComponent)
	{
		healthWidgetComponent->SetVisibility(bVisible);
	}
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerVisibility();

	if (!bShowDetectionDebug) return;

	//부채꼴 시각화, 가장자리 선과 중심선 표시
	const FVector origin = GetActorLocation();
	const FVector forward = GetActorForwardVector();
	const float halfAngle = detectionAngle * 0.5f;
	const FRotator yawL(0.f, -halfAngle, 0.f);
	const FRotator yawR(0.f, +halfAngle, 0.f);

	DrawDebugLine(GetWorld(), origin, origin + forward * detectionRadius, FColor::Yellow, false, -1.f, 0, 1.f);
	DrawDebugLine(GetWorld(), origin, origin + yawL.RotateVector(forward) * detectionRadius, FColor::Red, false, -1.f, 0, 1.f);
	DrawDebugLine(GetWorld(), origin, origin + yawR.RotateVector(forward) * detectionRadius, FColor::Red, false, -1.f, 0, 1.f);
}

bool AEnemyBase::IsInDetectionFan(const FVector& worldPoint) const
{
	//시야 차단 검사는 여기서 수행하지 않음
	const FVector toTarget = worldPoint - GetActorLocation();
	const float distSq = toTarget.SizeSquared();

	if (distSq <= KINDA_SMALL_NUMBER) return true;
	if (distSq > detectionRadius * detectionRadius) return false;

	//전방위(360도)면 각도 검사 스킵
	if (detectionAngle >= 360.f) return true;

	const FVector forward = GetActorForwardVector();
	const FVector dir = toTarget * FMath::InvSqrt(distSq);

	const float cosHalfAngle = FMath::Cos(FMath::DegreesToRadians(detectionAngle * 0.5f));
	return FVector::DotProduct(forward, dir) >= cosHalfAngle;
}

void AEnemyBase::ReceiveDamage(int32 Amount, bool bIsLethal)
{
	Super::ReceiveDamage(Amount, bIsLethal);

	//사망(은밀 처치)이나 실피해 없음은 전환 없음
	if (IsDead() || Amount <= 0) return;

	//상대 진영에게 피격당한 경우만 전투 전환
	ACharacterBase* attacker = GetLastAttacker().Get();
	if (!attacker || attacker->IsAlly() == IsAlly()) return;

	if (AEnemyAIController* aic = Cast<AEnemyAIController>(GetController()))
	{
		if (!aic->IsInCombat())
		{
			aic->JoinCombat(EJoinCombatReason::Attacked);
		}
	}
}

void AEnemyBase::HandleDeath()
{
	//lastAttacker를 Ally로 캐스팅, 경험치 분배는 아군 처치 한정
	AAllyCharacterBase* killer = Cast<AAllyCharacterBase>(lastAttacker.Get());

	UE_LOG(LogTemp, Log, TEXT("[EnemyBase] %s 사망 — 처치자: %s"),
		*GetName(),
		killer ? *killer->GetName() : TEXT("없음"));

	//경험치 분배, Destroy 전에 호출해야 유효한 참조 전달
	OnEnemyDeath.Broadcast(this, killer);

	Super::HandleDeath();
}
