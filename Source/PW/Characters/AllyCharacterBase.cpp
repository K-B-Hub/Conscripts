// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacterBase.h"
#include "PW.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/BattleGameMode.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Object/Buff/BuffBase.h"
#include "ActorComponent/BuffComponent.h"
#include "ActorComponent/TerrainComponent.h"
#include "Actors/Terrain/TerrainBase.h"
#include "Widget/HealthWidget.h"
#include "Widget/CampDialogueWidget.h"
#include "Components/WidgetComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/AnimSequence.h"

//패시브·버프·지형 공통 스탯 필드의 가감을 n번만큼 스냅샷에서 뺀다
template <typename TSource>
static void SubtractCommonStatDelta(FAllyRunState& state, const TSource* src, int32 n)
{
	state.MaxHp -= src->hp * n;
	state.Atk -= src->atk * n;
	state.Speed -= src->speed * n;
	state.Skill -= src->skill * n;
	state.Def -= src->def * n;
	state.Mentality -= src->mentality * n;
	state.MovingPoint -= src->movingPoint * n;
	state.ActionPoint -= src->actionPoint * n;
	state.DamageReduction -= src->damageReduction * n;
	state.DamageAmplification -= src->damageAmplification * n;
	state.Penetration -= src->penetration * n;
	state.Sight -= src->sight * n;
}

AAllyCharacterBase::AAllyCharacterBase()
{
	campDialogueComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("CampDialogueWidget"));
	campDialogueComponent->SetupAttachment(RootComponent);
	campDialogueComponent->SetRelativeLocation(FVector(0.f, 0.f, 160.f)); //체력 바보다 위
	campDialogueComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//말풍선은 WBP의 고정 크기(360×149, 텍스처 비율) 상자, 영역은 그보다 넉넉히 잡는다
	campDialogueComponent->SetDrawSize(FVector2D(380.f, 170.f));
	//앵커가 아래 변 가운데라 말풍선이 머리 위로 자란다
	campDialogueComponent->SetPivot(FVector2D(0.5f, 1.f));
	//navObstacle이 이 위젯 위치를 형상으로 잡으면 통행 불가 영역이 머리 위로 뜬다
	campDialogueComponent->SetCanEverAffectNavigation(false);

	//야영지에서 마우스를 올렸을 때만 켠다
	campDialogueComponent->SetVisibility(false);
}

void AAllyCharacterBase::ShowCampLine(const FText& line)
{
	if (!campDialogueComponent) return;

	//대사 풀이 비어 있으면 빈 말풍선을 띄우지 않는다
	if (line.IsEmpty())
	{
		HideCampLine();
		return;
	}

	if (UCampDialogueWidget* bubble = Cast<UCampDialogueWidget>(campDialogueComponent->GetWidget()))
	{
		bubble->SetLine(GetCharacterNameText(), line);
	}
	campDialogueComponent->SetVisibility(true);
}

void AAllyCharacterBase::HideCampLine()
{
	if (campDialogueComponent) campDialogueComponent->SetVisibility(false);
}

void AAllyCharacterBase::HideHealthBar()
{
	if (healthWidgetComponent) healthWidgetComponent->SetVisibility(false);
}

void AAllyCharacterBase::RestAtCamp(int32 healAmount, int32 stressRelief)
{
	hp = FMath::Clamp(hp + healAmount, 1, maxHp);
	RelieveStress(stressRelief);

	if (healthWidgetComponent)
	{
		if (UHealthWidget* healthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
		{
			healthWidget->InitHealth(maxHp, hp);
		}
	}
	OnVitalsChanged.Broadcast();
}

FAllyRunState AAllyCharacterBase::CaptureRunState() const
{
	FAllyRunState state;

	//클래스가 곧 캐릭터 한 명이라 이름·초상화는 스냅샷에 담지 않고 CDO에서 읽는다
	state.AllyClass = GetClass();

	state.Level = level;
	state.Exp = exp;
	state.MaxExp = maxExp;

	state.MaxHp = maxHp;
	state.Atk = atk;
	state.Speed = speed;
	state.Skill = skill;
	state.Def = def;
	state.Mentality = mentality;
	state.MovingPoint = movingPoint;
	state.ActionPoint = actionPoint;
	state.MaxStress = maxStress;
	state.MaxBattleResource = maxBattleResource;
	state.DamageReduction = damageReduction;
	state.DamageAmplification = damageAmplification;
	state.Penetration = penetration;
	state.Sight = sight;
	state.CriticalDamage = criticalDamage;

	state.Hp = hp;
	state.Stress = stress;
	state.BattleResource = battleResource;

	state.AcquiredUpgrades = acquiredUpgrades;
	state.SkillSlots = skillSlots;

	//턴이 오지 않아 못 고른 강화, 저장하지 않으면 레벨만 오른 채 선택권이 사라진다
	state.PendingUpgradeLevels = pendingUpgradeLevels;

	//조건부 패시브 가감은 전장 상황값이라 빼고 저장, 복원 때 다시 평가해 건다
	for (const TPair<TWeakObjectPtr<const UPassiveSkillBase>, int32>& pair : GetPassiveDeltaStacks())
	{
		const UPassiveSkillBase* p = pair.Key.Get();
		if (!p) continue;
		const int32 n = pair.Value;
		SubtractCommonStatDelta(state, p, n);
		state.Hp -= p->hp * n;
		state.MaxBattleResource -= p->maxBattleResource * n;
		state.BattleResource -= p->maxBattleResource * n;
		state.CriticalDamage -= p->criticalDamage * n;
	}

	//버프·지형 가감도 전투 중 일시값이라 빼고 저장
	if (const UBuffComponent* bc = GetBuffComponent())
	{
		for (const UBuffBase* b : bc->GetActiveBuffs())
		{
			if (!b) continue;
			SubtractCommonStatDelta(state, b, 1);
			state.CriticalDamage -= b->criticalDamage;
		}
	}
	if (const UTerrainComponent* tc = GetTerrainComponent())
	{
		for (const ATerrainBase* t : tc->GetCurrentTerrains())
		{
			if (t) SubtractCommonStatDelta(state, t, 1);
		}
	}

	return state;
}

void AAllyCharacterBase::RestoreRunState(const FAllyRunState& state)
{
	//1) 습득 강화 재등록. 리액티브·컨디셔널 훅과 컴포넌트의 파생 스탯 보너스가 여기서만 복원된다
	//   Stat 패시브가 베이스 스탯을 다시 가산하지만 2)의 대입이 통째로 덮어쓴다
	for (const TSubclassOf<USkillBase>& upgrade : state.AcquiredUpgrades)
	{
		AcquireUpgrade(upgrade);
	}

	//AcquireUpgrade가 목록에 다시 쌓으므로 스냅샷 값으로 되돌린다
	acquiredUpgrades = state.AcquiredUpgrades;

	//스킬 슬롯 배치, 보유 스킬과의 정합은 InitSkills의 정규화가 맡는다
	skillSlots = state.SkillSlots;

	//못 고른 강화 큐, 다음 전투 첫 턴의 InitTurn이 기존 경로로 소비한다
	pendingUpgradeLevels = state.PendingUpgradeLevels;

	//조건부 패시브가 1)과 BeginPlay에서 건 가감을 걷어 둔다, 2)의 대입이 지우면 래치만 켜진 채 남는다
	const TMap<TWeakObjectPtr<const UPassiveSkillBase>, int32> conditionalStacks = GetPassiveDeltaStacks();
	for (const TPair<TWeakObjectPtr<const UPassiveSkillBase>, int32>& pair : conditionalStacks)
	{
		for (int32 i = 0; i < pair.Value; ++i) ApplyPassiveStatDelta(pair.Key.Get(), -1);
	}
	//스폰 시 겹친 지형과 그 진입 버프도 같은 이유로 걷어 둔다
	TArray<TObjectPtr<UBuffBase>> buffs;
	if (UBuffComponent* bc = GetBuffComponent()) buffs = bc->GetActiveBuffs();
	for (const UBuffBase* b : buffs) ApplyBuffDelta(b, -1);
	TArray<TObjectPtr<ATerrainBase>> terrains;
	if (UTerrainComponent* tc = GetTerrainComponent()) terrains = tc->GetCurrentTerrains();
	for (const ATerrainBase* t : terrains) ApplyTerrainStatDelta(t, -1);

	//2) 베이스 스탯 대입. 대입은 멱등이라 BeginPlay 기본 패시브와 1)의 이중 가산이 함께 사라진다
	level = state.Level;
	exp = state.Exp;
	maxExp = state.MaxExp;

	maxHp = state.MaxHp;
	atk = state.Atk;
	speed = state.Speed;
	skill = state.Skill;
	def = state.Def;
	mentality = state.Mentality;
	movingPoint = state.MovingPoint;
	actionPoint = state.ActionPoint;
	maxStress = state.MaxStress;
	maxBattleResource = state.MaxBattleResource;
	damageReduction = state.DamageReduction;
	damageAmplification = state.DamageAmplification;
	penetration = state.Penetration;
	sight = state.Sight;
	criticalDamage = state.CriticalDamage;

	hp = FMath::Clamp(state.Hp, 1, maxHp);
	stress = state.Stress;
	battleResource = FMath::Clamp(state.BattleResource, 0, maxBattleResource);

	//걷어 둔 조건부·버프·지형 가감을 대입된 베이스 위에 다시 건다
	for (const TPair<TWeakObjectPtr<const UPassiveSkillBase>, int32>& pair : conditionalStacks)
	{
		for (int32 i = 0; i < pair.Value; ++i) ApplyPassiveStatDelta(pair.Key.Get(), +1);
	}
	for (const UBuffBase* b : buffs) ApplyBuffDelta(b, +1);
	for (const ATerrainBase* t : terrains) ApplyTerrainStatDelta(t, +1);

	//턴 시작 전이므로 현재치는 최대치에서 출발
	currentMovingPoint = movingPoint;
	currentActionPoint = actionPoint;

	//3) 파생 스탯 재계산. 컴포넌트에 누적된 명중·회피·치명 보너스가 여기서 반영된다
	SetDefaultStats();

	if (healthWidgetComponent)
	{
		if (UHealthWidget* healthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
		{
			healthWidget->InitHealth(maxHp, hp);
		}
	}
	OnVitalsChanged.Broadcast();
}

void AAllyCharacterBase::PlayRandomCampIdle()
{
	//지정되지 않은 직업은 기본 대기 포즈로 둔다
	UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
	if (campIdleAnims.Num() == 0 || !animInstance) return;

	UAnimSequence* picked = campIdleAnims[FMath::RandRange(0, campIdleAnims.Num() - 1)];
	if (!picked) return;

	//섹션을 자기 자신으로 이어 무한 반복
	UAnimMontage* montage = animInstance->PlaySlotAnimationAsDynamicMontage(picked, TEXT("DefaultSlot"), 0.25f, 0.25f);
	if (!montage) return;
	const FName section = montage->GetSectionName(0);
	animInstance->Montage_SetNextSection(section, section, montage);
}

void AAllyCharacterBase::InitTurn()
{
	Super::InitTurn();

	//대기 중인 레벨업 강화가 있으면 알림, 위젯 생성·잠금은 컨트롤러 책임
	//턴 강제 종료 예약·상태이상 점유 시 보류, 대기 큐가 유지되어 다음 행동 가능한 턴에 다시 표시됨
	if (pendingUpgradeLevels.Num() > 0 && !bTurnEndRequested && !bAilmentDrivenTurn)
	{
		OnUpgradeSelectRequested.Broadcast();
	}
}

void AAllyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//아치 궤적 추종 중이면 걷기 이동 처리 생략 (추종은 CharacterBase::Tick이 담당)
	if (bIsArcMoving) return;

	if (!bIsMovingToTarget || pathPoints.Num() == 0) return;

	const FVector CurrentLoc = GetActorLocation();

	//이전 프레임 대비 이동한 거리를 미터로 변환해 이동력 차감, 지형 배율·포복 자세 배율 반영
	const float MovedCm = FVector::Dist(CurrentLoc, lastFrameLocation);
	ConsumeMovingPoint(MovedCm / CmPerMeter * GetTerrainMoveCostMultiplier() * GetStanceMoveCostMultiplier());
	lastFrameLocation = CurrentLoc;

	//이동력 소진 시 즉시 정지 후 자연 종료 알림
	if (currentMovingPoint <= 0.f)
	{
		StopMovement();
		//MoveComplete 패시브 발동을 OnMovementCompleted 브로드캐스트 전에 처리
		if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			GM->BroadcastMoveComplete();
		}
		OnMovementCompleted.Broadcast();
		return;
	}

	const FVector Target = pathPoints[pathPointIndex];
	const FVector Delta = Target - CurrentLoc;
	const float Dist2D = FVector2D(Delta.X, Delta.Y).Size();

	//현재 경유점 도달 판정
	if (Dist2D < 5.f)
	{
		pathPointIndex++;

		//마지막 경유점 도달 시 정확한 위치에 스냅 후 이동 종료, 자연 종료 알림
		if (pathPointIndex >= pathPoints.Num())
		{
			SetActorLocation(FVector(moveDestination.X, moveDestination.Y, CurrentLoc.Z));
			GetCharacterMovement()->StopMovementImmediately();
			bIsMovingToTarget = false;
			//MoveComplete 패시브 발동을 OnMovementCompleted 브로드캐스트 전에 처리
			if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
			{
				GM->BroadcastMoveComplete();
			}
			OnMovementCompleted.Broadcast();
			return;
		}
	}

	//다음 경유점 방향으로 입력, 목적지 근접 시 감속
	const FVector MoveDir = FVector(Delta.X, Delta.Y, 0.f).GetSafeNormal();

	const float distToDestination = FVector2D(moveDestination.X - CurrentLoc.X,
	                                           moveDestination.Y - CurrentLoc.Y).Size();
	const float moveScale = (distToDestination < moveDecelRadius)
		? FMath::Max(0.15f, distToDestination / moveDecelRadius)
		: 1.0f;

	AddMovementInput(MoveDir, moveScale);
}

void AAllyCharacterBase::EndTurn()
{
	Super::EndTurn();
	StopMovement();
}

void AAllyCharacterBase::HandleDeath()
{
	//이동 중이면 정지
	StopMovement();

	//델리게이트 바인딩 정리, 소멸자에서 정리 시 크래시 방지
	OnMovementCompleted.Clear();

	Super::HandleDeath();
}

void AAllyCharacterBase::MoveAlongPath(const TArray<FVector>& Points)
{
	StopMovement();
	if (Points.Num() == 0) return;

	//실제 이동 시작, BeforeMove 전이
	OnMoveStateChanged(true);

	pathPoints = Points;
	pathPointIndex = 0;
	moveDestination = Points.Last();
	lastFrameLocation = GetActorLocation(); //첫 프레임 거리 오차 방지
	bIsMovingToTarget = true;
}

void AAllyCharacterBase::MoveAlongArc(const TArray<FVector>& ArcPoints, bool bForced)
{
	if (bIsArcMoving) return;

	//진행 중이던 걷기 이동은 정리
	bIsMovingToTarget = false;
	pathPoints.Empty();

	Super::MoveAlongArc(ArcPoints, bForced);
}

void AAllyCharacterBase::NotifyArcMoveCompleted()
{
	OnMovementCompleted.Broadcast();
}

void AAllyCharacterBase::StopMovement()
{
	bIsMovingToTarget = false;
	pathPoints.Empty();
	GetCharacterMovement()->StopMovementImmediately();
}
