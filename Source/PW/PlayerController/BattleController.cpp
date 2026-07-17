// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/AllyCharacterBase.h"
#include "Actors/CursorIndicator.h"
#include "Widget/BattleTurnWidget.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "GameMode/BattleGameMode.h"
#include "Characters/CharacterBase.h"
#include "Characters/EnemyBase.h"
#include "Enum/SkillTypes.h"
#include "Components/CapsuleComponent.h"
#include "Actors/AttackRangeIndicator.h"
#include "Landscape.h"

ABattleController::ABattleController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	PrimaryActorTick.bCanEverTick = true;
}

void ABattleController::BeginPlay()
{
	Super::BeginPlay();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (battleInputMappingContext)
		{
			Subsystem->AddMappingContext(battleInputMappingContext, 0);
		}
	}

	//카메라 폰 스프링암 캐시 및 초기 Yaw/Pitch 동기화
	if (USpringArmComponent* SpringArm = ResolveSpringArm())
	{
		const FRotator InitRot = SpringArm->GetRelativeRotation();
		currentCameraYaw     = InitRot.Yaw;
		cachedSpringArmPitch = InitRot.Pitch;
	}
}

USpringArmComponent* ABattleController::ResolveSpringArm()
{
	//카메라 폰은 상시 빙의라 최초 1회만 재캐시 발생
	if (!IsValid(cachedSpringArm))
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			cachedSpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>();
		}
	}
	return cachedSpringArm;
}

void ABattleController::InitTurn(AAllyCharacterBase* TurnUnit)
{
	if (!IsValid(TurnUnit)) return;

	activeUnit = TurnUnit;
	aiFollowTarget = nullptr; //아군 턴 진입, 적 추적 해제
	if (ResolveSpringArm() == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleController] InitTurn: 카메라 폰의 SpringArmComponent를 찾을 수 없습니다."));
		return;
	}
	activeUnit->InitTurn();
	activeUnit->SetNavObstacleEnabled(false); //본인 경로 계산 시 자신이 장애물이 되지 않도록
	OnCameraReset(FInputActionValue()); //카메라 초기화 및 추적 모드 활성화

	//이동 완료 델리게이트 바인딩, EndTurn에서 해제
	activeUnit->OnMovementCompleted.AddUObject(this, &ABattleController::OnUnitMovementCompleted);

	//턴 HUD 생성, 이동/스킬/턴종료/게이지 위젯 일괄 구성
	if (turnHudWidgetClass)
	{
		turnHudWidgetInstance = CreateWidget<UBattleTurnWidget>(this, turnHudWidgetClass);
		if (turnHudWidgetInstance)
		{
			turnHudWidgetInstance->AddToViewport();
			turnHudWidgetInstance->InitTurnHUD(activeUnit);
		}
	}

	//턴 중 hp/스트레스 변동 반영, EndTurn에서 해제
	activeUnit->OnVitalsChanged.AddUObject(this, &ABattleController::RefreshGauges);
}

void ABattleController::EndTurn()
{
	if (IsValid(activeUnit))
	{
		//델리게이트 해제 후 이동 종료, 해제 먼저 해야 오발 방지
		activeUnit->OnMovementCompleted.RemoveAll(this);
		activeUnit->OnVitalsChanged.RemoveAll(this);
		activeUnit->SetNavObstacleEnabled(true); //턴 종료 후 다시 장애물로 등록
		ExitMoveMode();
		DeactivateSkill();
		activeUnit->EndTurn();
	}
	if (IsValid(turnHudWidgetInstance))
	{
		turnHudWidgetInstance->RemoveFromParent();
		turnHudWidgetInstance = nullptr;
	}
	activeUnit = nullptr;
}

void ABattleController::RefreshGauges()
{
	if (!IsValid(activeUnit) || !IsValid(turnHudWidgetInstance)) return;

	turnHudWidgetInstance->RefreshGauges(activeUnit->GetHp(), activeUnit->GetStress());
}

void ABattleController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EIC) return;

	if (iA_CameraMove)
	{
		EIC->BindAction(iA_CameraMove, ETriggerEvent::Triggered, this, &ABattleController::OnCameraMove);
	}
	if (iA_CameraRotate)
	{
		EIC->BindAction(iA_CameraRotate, ETriggerEvent::Triggered, this, &ABattleController::OnCameraRotate);
	}
	if (iA_CameraZoom)
	{
		EIC->BindAction(iA_CameraZoom, ETriggerEvent::Triggered, this, &ABattleController::OnCameraZoom);
	}
	if (iA_MoveCommand)
	{
		EIC->BindAction(iA_MoveCommand, ETriggerEvent::Started, this, &ABattleController::OnMoveCommand);
	}
	if (iA_CancelMove)
	{
		EIC->BindAction(iA_CancelMove, ETriggerEvent::Started, this, &ABattleController::OnCancelMove);
	}
	if (iA_CameraReset)
	{
		EIC->BindAction(iA_CameraReset, ETriggerEvent::Started, this, &ABattleController::OnCameraReset);
	}
}

void ABattleController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (USpringArmComponent* SpringArm = ResolveSpringArm())
	{
		//추적 대상은 아군 턴이면 activeUnit, 적 턴이면 aiFollowTarget
		AActor* followUnit = IsValid(activeUnit) ? static_cast<AActor*>(activeUnit) : nullptr;
		if (!followUnit) followUnit = IsValid(aiFollowTarget) ? static_cast<AActor*>(aiFollowTarget) : nullptr;

		//카메라 추적 모드, 스프링암 위치를 캐릭터 위치로 고정
		if (bIsFollowingCharacter && followUnit)
		{
			SpringArm->SetWorldLocation(followUnit->GetActorLocation());
			SnapSpringArmToGround(SpringArm);
		}
		//자유 이동 모드, 입력이 없으면 관성으로 감속
		else if (!bCameraInputActive && !cameraVelocity.IsNearlyZero(1.f))
		{
			cameraVelocity = FMath::VInterpTo(cameraVelocity, FVector::ZeroVector, DeltaTime, cameraMoveSmoothing);
			SpringArm->AddWorldOffset(cameraVelocity * DeltaTime);
			SnapSpringArmToGround(SpringArm);
		}
	}

	bCameraInputActive = false;
}

void ABattleController::OnCameraMove(const FInputActionValue& Value)
{
	USpringArmComponent* SpringArm = ResolveSpringArm();
	if (!SpringArm) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero()) return;

	//카메라를 움직이면 추적 모드 해제
	bIsFollowingCharacter = false;

	const FRotator YawRotation(0.f, currentCameraYaw, 0.f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	//목표 속도를 향해 부드럽게 가속
	const FVector TargetVelocity = (ForwardDir * MoveInput.Y + RightDir * MoveInput.X) * cameraMoveSpeed;
	cameraVelocity = FMath::VInterpTo(cameraVelocity, TargetVelocity, DeltaTime, cameraMoveSmoothing);

	SpringArm->AddWorldOffset(cameraVelocity * DeltaTime);
	SnapSpringArmToGround(SpringArm);

	bCameraInputActive = true;
}

void ABattleController::OnCameraRotate(const FInputActionValue& Value)
{
	USpringArmComponent* SpringArm = ResolveSpringArm();
	if (!SpringArm) return;

	const float RotateInput = Value.Get<float>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	currentCameraYaw += RotateInput * cameraRotateSpeed * DeltaTime;

	//Pitch는 고정, Yaw만 변경
	SpringArm->SetRelativeRotation(FRotator(cachedSpringArmPitch, currentCameraYaw, 0.f));
}

void ABattleController::OnCameraZoom(const FInputActionValue& Value)
{
	USpringArmComponent* SpringArm = ResolveSpringArm();
	if (!SpringArm) return;

	const float ZoomInput = Value.Get<float>();
	SpringArm->TargetArmLength = FMath::Clamp(
		SpringArm->TargetArmLength - ZoomInput * cameraZoomSpeed,
		cameraZoomMin,
		cameraZoomMax
	);
}

void ABattleController::OnMoveCommand(const FInputActionValue& Value)
{
	if (!activeUnit) return;

	//스킬 모드 우선 처리
	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (SkillComp && SkillComp->IsSkillActive())
	{
		ExecuteSkill();
		return;
	}

	//이동 모드
	if (!bIsMoveMode || !IsValid(cursorIndicatorInstance)) return;

	const TArray<FVector>& PathPoints = cursorIndicatorInstance->GetCachedPathPoints();
	if (PathPoints.Num() == 0) return;

	cursorIndicatorInstance->LockAtCurrentPosition();
	activeUnit->MoveAlongPath(PathPoints);
	UE_LOG(LogTemp, Log, TEXT("[BattleController] 이동 명령: %s (%d개 경유점)"),
		*PathPoints.Last().ToString(), PathPoints.Num());
}

void ABattleController::OnCancelMove(const FInputActionValue& Value)
{
	//스킬 모드 활성 시 스킬 취소 우선, 자동이동 중이면 이동도 중단
	if (activeUnit)
	{
		USkillComponent* SkillComp = activeUnit->GetSkillComponent();
		if (SkillComp && SkillComp->IsSkillActive())
		{
			if (bPendingSkillExec)
			{
				activeUnit->StopMovement();
				bPendingSkillExec = false;
			}
			SkillComp->DeactivateSkill();
			return;
		}
	}

	if (!bIsMoveMode || !activeUnit) return;

	activeUnit->StopMovement();
	ExitMoveMode();
	UE_LOG(LogTemp, Log, TEXT("[BattleController] 이동 취소"));
}

void ABattleController::OnCameraReset(const FInputActionValue& Value)
{
	AActor* followUnit = IsValid(activeUnit) ? static_cast<AActor*>(activeUnit) : nullptr;
	if (!followUnit) followUnit = IsValid(aiFollowTarget) ? static_cast<AActor*>(aiFollowTarget) : nullptr;
	USpringArmComponent* SpringArm = ResolveSpringArm();
	if (!SpringArm || !followUnit) return;

	//스프링암을 즉시 캐릭터 위치로 이동 후 추적 모드 활성화
	SpringArm->SetWorldLocation(followUnit->GetActorLocation());
	SnapSpringArmToGround(SpringArm);
	cameraVelocity = FVector::ZeroVector;
	bIsFollowingCharacter = true;

	//카메라 각도를 초기값으로 리셋, 턴 전환 시 이전 Yaw가 남는 문제 방지
	currentCameraYaw = 0.f;
	SpringArm->SetRelativeRotation(FRotator(cachedSpringArmPitch, currentCameraYaw, 0.f));
}

void ABattleController::BeginAITurnFollow(ACharacterBase* AIUnit)
{
	if (!IsValid(AIUnit)) return;

	aiFollowTarget = AIUnit;

	USpringArmComponent* SpringArm = ResolveSpringArm();
	if (!SpringArm) return;

	//즉시 AI 위치로 스냅 후 추적 모드 활성화
	SpringArm->SetWorldLocation(AIUnit->GetActorLocation());
	SnapSpringArmToGround(SpringArm);
	cameraVelocity = FVector::ZeroVector;
	bIsFollowingCharacter = true;
}

void ABattleController::ActivateSkill(UActiveSkillBase* Skill)
{
	if (!activeUnit || !Skill) return;

	//이동 모드와 상호 배타
	if (bIsMoveMode) ExitMoveMode();

	//멀티픽 초기화
	remainingPicks = (Skill->selectMode == ESelectMode::SinglePick && Skill->pickCount > 1)
		? Skill->pickCount : 0;

	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (SkillComp)
	{
		SkillComp->ActivateSkill(Skill);
	}
}

void ABattleController::DeactivateSkill()
{
	if (!activeUnit) return;

	bPendingSkillExec = false;
	remainingPicks = 0;

	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (SkillComp && SkillComp->IsSkillActive())
	{
		SkillComp->DeactivateSkill();
	}
}

void ABattleController::ToggleMoveMode()
{
	if (activeUnit->GetCurrentMovingPoint() <= 0.f)
	{
		return;
	}
	if (bIsMoveMode) ExitMoveMode();
	else             EnterMoveMode();
}

void ABattleController::EnterMoveMode()
{
	if (bIsMoveMode || !activeUnit || !cursorIndicatorClass) return;

	//스킬 모드와 상호 배타
	DeactivateSkill();

	cursorIndicatorInstance = GetWorld()->SpawnActor<ACursorIndicator>(cursorIndicatorClass);
	if (cursorIndicatorInstance)
	{
		cursorIndicatorInstance->SetActiveUnit(activeUnit);
	}
	bIsMoveMode = true;
}

void ABattleController::ExitMoveMode()
{
	if (!bIsMoveMode) return;

	if (IsValid(cursorIndicatorInstance))
	{
		cursorIndicatorInstance->Destroy();
		cursorIndicatorInstance = nullptr;
	}
	bIsMoveMode = false;
}

void ABattleController::ResetCursorIndicator()
{
	//잠금 해제만으로 마우스 추적 재개, 스폰/파괴 불필요
	if (IsValid(cursorIndicatorInstance))
	{
		cursorIndicatorInstance->Unlock();
	}
}

void ABattleController::OnUnitMovementCompleted()
{
	if (!activeUnit) return;

	//자동이동 후 스킬 실행, 이동 전 경로 검증 완료라 바로 실행
	if (bPendingSkillExec)
	{
		bPendingSkillExec = false;
		USkillComponent* SkillComp = activeUnit->GetSkillComponent();
		if (!SkillComp) return;

		AAttackRangeIndicator* Indicator = SkillComp->GetAttackRangeIndicator();

		//인디케이터 방향으로 캐릭터 회전, Self는 제외
		UActiveSkillBase* Skill = SkillComp->GetCurrentSkill();
		if (!Skill) return;

		if (Indicator && Skill->selectMode != ESelectMode::Self)
		{
			FVector Dir = Indicator->GetActorLocation() - activeUnit->GetActorLocation();
			Dir.Z = 0.f;
			if (!Dir.IsNearlyZero())
			{
				activeUnit->SetActorRotation(Dir.Rotation());
			}
		}

		if (Indicator) Indicator->Unlock();

		const TArray<ACharacterBase*> Targets = SkillComp->GetCurrentTargets();
		if (Targets.Num() == 0 && Skill->selectMode == ESelectMode::SinglePick) return;

		Skill->BeginUse();
		if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			GM->RecordSkillUse(activeUnit, Skill);
		}

		for (ACharacterBase* Target : Targets)
		{
			if (!IsValid(Target)) continue;
			Target->SetLastAttacker(activeUnit);
			if (!Target->ReflectDamage()) continue;
			if (!IsValid(Target)) continue;
			Skill->Execute(Target);
		}
		SkillComp->DeactivateSkill();
		RefreshSkillButtons();

		//자동이동으로 소비된 이동력 반영, Move 버튼 상태 갱신
		if (IsValid(turnHudWidgetInstance))
		{
			turnHudWidgetInstance->RefreshMoveButton(activeUnit->GetCurrentMovingPoint());
		}

		UE_LOG(LogTemp, Log, TEXT("[BattleController] 자동이동 후 스킬 실행: %s → %d명 대상"),
			*Skill->skillName.ToString(), Targets.Num());
		return;
	}

	//이동 완료 후 이동력 잔여 여부로 버튼 상태 갱신
	if (IsValid(turnHudWidgetInstance))
	{
		turnHudWidgetInstance->RefreshMoveButton(activeUnit->GetCurrentMovingPoint());
	}

	//이동 모드 처리
	if (!bIsMoveMode) return;

	if (activeUnit->GetCurrentMovingPoint() <= 0.f)
	{
		//이동력 소진, 이동 모드 자동 종료
		ExitMoveMode();
	}
	else
	{
		//이동력 잔여, 커서 인디케이터 리셋해 다음 이동 준비
		ResetCursorIndicator();
	}
}

void ABattleController::ExecuteSkill()
{
	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (!SkillComp) return;

	UActiveSkillBase* Skill = SkillComp->GetCurrentSkill();
	if (!Skill) return;

	AAttackRangeIndicator* Indicator = SkillComp->GetAttackRangeIndicator();

	//SinglePick은 스냅 상태 검증
	if (Skill->selectMode == ESelectMode::SinglePick)
	{
		if (!Indicator || !Indicator->GetSnappedTarget()) return;
	}

	//멀티픽 모드(pickCount > 1, SinglePick), 실제 영향 대상은 EAreaTarget 기반 오버랩
	if (Skill->selectMode == ESelectMode::SinglePick && Skill->pickCount > 1)
	{
		ACharacterBase* SnappedTarget = Indicator->GetSnappedTarget();
		if (!SnappedTarget) return;

		//사거리 밖 대상은 선택 불가
		if (Indicator->ComputeOutOfRange()) return;

		//현재 오버랩 대상(EAreaTarget 필터 적용됨)을 누적 저장
		SkillComp->AccumulateCurrentTargets();
		remainingPicks--;

		UE_LOG(LogTemp, Log, TEXT("[BattleController] 멀티픽: %s 선택 (남은 선택: %d, 누적 대상: %d)"),
			*SnappedTarget->GetName(), remainingPicks, SkillComp->GetAccumulatedTargets().Num());

		if (remainingPicks > 0) return;

		//모든 선택 완료, 누적된 영향 대상으로 실행
		if (Indicator)
		{
			FVector Dir = Indicator->GetActorLocation() - activeUnit->GetActorLocation();
			Dir.Z = 0.f;
			if (!Dir.IsNearlyZero())
			{
				activeUnit->SetActorRotation(Dir.Rotation());
			}
		}

		//멀티픽 누적 대상 중 사망한 캐릭터 필터링
		TArray<ACharacterBase*> ValidTargets;
		for (ACharacterBase* Target : SkillComp->GetAccumulatedTargets())
		{
			if (IsValid(Target))
			{
				ValidTargets.Add(Target);
			}
		}

		Skill->BeginUse();
		if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
		{
			GM->RecordSkillUse(activeUnit, Skill);
		}

		for (ACharacterBase* Target : ValidTargets)
		{
			if (!IsValid(Target)) continue;
			Target->SetLastAttacker(activeUnit);
			if (!Target->ReflectDamage()) continue;
			if (!IsValid(Target)) continue;
			Skill->Execute(Target);
		}
		remainingPicks = 0;
		SkillComp->DeactivateSkill();
		RefreshSkillButtons();

		return;
	}

	//사거리 밖은 이동력이 충분한 경우에만 자동이동, 멀티픽은 자동이동 불가
	if (Indicator && Indicator->ComputeOutOfRange())
	{
		const TArray<FVector>& MovePath = Indicator->GetMovePath();
		if (MovePath.Num() < 2) return;

		//경로 총 거리 계산
		float pathDistanceCm = 0.f;
		for (int32 i = 0; i + 1 < MovePath.Num(); ++i)
		{
			pathDistanceCm += FVector::Dist(MovePath[i], MovePath[i + 1]);
		}

		//이동력 부족 시 자동이동 불가
		const float budgetCm = activeUnit->GetCurrentMovingPoint() * 100.f;
		if (pathDistanceCm > budgetCm) return;

		Indicator->LockAtCurrentPosition();
		bPendingSkillExec = true;
		activeUnit->MoveAlongPath(MovePath);
		return;
	}

	//인디케이터 방향으로 캐릭터 회전, Self는 제외
	if (Indicator && Skill->selectMode != ESelectMode::Self)
	{
		FVector Dir = Indicator->GetActorLocation() - activeUnit->GetActorLocation();
		Dir.Z = 0.f;
		if (!Dir.IsNearlyZero())
		{
			activeUnit->SetActorRotation(Dir.Rotation());
		}
	}

	//스킬 영향 대상은 오버랩 단계에서 EAreaTarget으로 이미 필터된 SkillComponent 목록 사용
	const TArray<ACharacterBase*> Targets = SkillComp->GetCurrentTargets();
	//SinglePick만 타겟 필수, Self/GroundPoint는 타겟 없이도 실행 가능
	if (Targets.Num() == 0 && Skill->selectMode == ESelectMode::SinglePick) return;

	Skill->BeginUse();
	if (ABattleGameMode* GM = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		GM->RecordSkillUse(activeUnit, Skill);
	}

	//전투 예측 값은 이미 오버랩 시 CalculateDamage로 세팅됨, 바로 ReflectDamage
	for (ACharacterBase* Target : Targets)
	{
		if (!IsValid(Target)) continue;
		Target->SetLastAttacker(activeUnit);
		if (!Target->ReflectDamage()) continue;
		if (!IsValid(Target)) continue;
		Skill->Execute(Target);
	}

	//스킬 사용 완료, 비활성화
	SkillComp->DeactivateSkill();
	RefreshSkillButtons();

	UE_LOG(LogTemp, Log, TEXT("[BattleController] 스킬 실행: %s → %d명 대상"),
		*Skill->skillName.ToString(), Targets.Num());
}

void ABattleController::RefreshSkillButtons()
{
	if (IsValid(turnHudWidgetInstance))
	{
		turnHudWidgetInstance->RefreshSkillButtons();
	}
}

void ABattleController::SnapSpringArmToGround(USpringArmComponent* SpringArm)
{
	const FVector PivotPos = SpringArm->GetComponentLocation();

	//랜드스케이프만 감지하기 위해 멀티 트레이스 후 필터링
	TArray<FHitResult> Hits;
	GetWorld()->LineTraceMultiByChannel(
		Hits,
		PivotPos + FVector(0.f, 0.f, 500.f),
		PivotPos - FVector(0.f, 0.f, 500.f),
		ECC_Visibility);

	for (const FHitResult& Hit : Hits)
	{
		if (Hit.GetActor() && Hit.GetActor()->IsA<ALandscapeProxy>())
		{
			SpringArm->SetWorldLocation(FVector(PivotPos.X, PivotPos.Y, Hit.Location.Z + cameraGroundOffset));
			return;
		}
	}
}

