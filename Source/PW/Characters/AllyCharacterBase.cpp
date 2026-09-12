// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/AllyCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/BattleGameMode.h"
#include "Widget/HealthWidget.h"
#include "Components/WidgetComponent.h"

FAllyRunState AAllyCharacterBase::CaptureRunState() const
{
	FAllyRunState state;

	state.AllyClass = GetClass();
	state.DisplayName = displayName;

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

	return state;
}

void AAllyCharacterBase::RestoreRunState(const FAllyRunState& state)
{
	displayName = state.DisplayName;

	//1) 습득 강화 재등록. 리액티브·컨디셔널 훅과 컴포넌트의 파생 스탯 보너스가 여기서만 복원된다
	//   Stat 패시브가 베이스 스탯을 다시 가산하지만 2)의 대입이 통째로 덮어쓴다
	for (const TSubclassOf<USkillBase>& upgrade : state.AcquiredUpgrades)
	{
		AcquireUpgrade(upgrade);
	}

	//AcquireUpgrade가 목록에 다시 쌓으므로 스냅샷 값으로 되돌린다
	acquiredUpgrades = state.AcquiredUpgrades;

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
	ConsumeMovingPoint(MovedCm / 100.f * GetTerrainMoveCostMultiplier() * GetStanceMoveCostMultiplier());
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