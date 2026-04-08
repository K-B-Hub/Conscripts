// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/AllyCharacterBase.h"
#include "Actors/CursorIndicator.h"
#include "Widget/TurnEndWidget.h"
#include "Widget/SkillWidget.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Characters/CharacterBase.h"
#include "Characters/EnemyBase.h"
#include "Enum/SkillTypes.h"
#include "Components/CapsuleComponent.h"

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

	// 초기 카메라 Yaw / Pitch를 스프링암 기준으로 동기화
	if (APawn* ControlledPawn = GetPawn())
	{
		if (USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>())
		{
			const FRotator InitRot = SpringArm->GetRelativeRotation();
			currentCameraYaw     = InitRot.Yaw;
			cachedSpringArmPitch = InitRot.Pitch;
		}
	}

}

void ABattleController::InitTurn(AAllyCharacterBase* TurnUnit)
{
	if (!IsValid(TurnUnit)) return;

	activeUnit = TurnUnit;
	cachedSpringArm = activeUnit
		? activeUnit->FindComponentByClass<USpringArmComponent>()
		: nullptr;
	if (cachedSpringArm == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BattleController] InitTurn: 유효한 SpringArmComponent를 찾을 수 없습니다."));
		return;
	}
	activeUnit->InitTurn();
	activeUnit->SetNavObstacleEnabled(false); // 본인 경로 계산 시 자신이 장애물이 되지 않도록
	OnCameraReset(FInputActionValue()); // 카메라 초기화 및 추적 모드 활성화

	// 이동 완료 델리게이트 바인딩 (EndTurn에서 해제)
	activeUnit->OnMovementCompleted.AddUObject(this, &ABattleController::OnUnitMovementCompleted);

	// 턴 종료 위젯 생성 및 뷰포트 추가
	if (turnEndWidgetClass)
	{
		turnEndWidgetInstance = CreateWidget<UTurnEndWidget>(this, turnEndWidgetClass);
		if (turnEndWidgetInstance)
		{
			turnEndWidgetInstance->AddToViewport();
		}
	}

	// 스킬 위젯 생성 — SkillComponent의 액티브 스킬로 버튼 배치
	if (skillWidgetClass)
	{
		skillWidgetInstance = CreateWidget<USkillWidget>(this, skillWidgetClass);
		if (skillWidgetInstance)
		{
			if (USkillComponent* SkillComp = activeUnit->GetSkillComponent())
			{
				SkillComp->CalcSkillStats();
				skillWidgetInstance->InitSkills(SkillComp);
			}
			skillWidgetInstance->AddToViewport();
		}
	}
}

void ABattleController::EndTurn()
{
	if (IsValid(activeUnit))
	{
		// 델리게이트 해제 후 이동 종료 (해제 먼저 해야 OnUnitMovementCompleted 오발 방지)
		activeUnit->OnMovementCompleted.RemoveAll(this);
		activeUnit->SetNavObstacleEnabled(true); // 턴 종료 후 다시 장애물로 등록
		ExitMoveMode();
		DeactivateSkill();
		activeUnit->EndTurn();
	}
	if (IsValid(turnEndWidgetInstance))
	{
		turnEndWidgetInstance->RemoveFromParent();
		turnEndWidgetInstance = nullptr;
	}
	if (IsValid(skillWidgetInstance))
	{
		skillWidgetInstance->RemoveFromParent();
		skillWidgetInstance = nullptr;
	}
	activeUnit = nullptr;
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

// ─── Tick: 캐릭터 추적 / 감속 / 지면 스냅 ───────────────────────────────────
void ABattleController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (cachedSpringArm)
	{
		// 카메라 추적 모드: 스프링암 위치를 캐릭터 위치로 고정
		if (bIsFollowingCharacter && activeUnit)
		{
			cachedSpringArm->SetWorldLocation(activeUnit->GetActorLocation());
			SnapSpringArmToGround(cachedSpringArm);
		}
		// 자유 이동 모드: 입력이 없으면 관성으로 감속
		else if (!bCameraInputActive && !cameraVelocity.IsNearlyZero(1.f))
		{
			cameraVelocity = FMath::VInterpTo(cameraVelocity, FVector::ZeroVector, DeltaTime, cameraMoveSmoothing);
			cachedSpringArm->AddWorldOffset(cameraVelocity * DeltaTime);
			SnapSpringArmToGround(cachedSpringArm);
		}
	}

	bCameraInputActive = false;
}

// ─── 카메라 이동 ──────────────────────────────────────────────────────────────
void ABattleController::OnCameraMove(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();
	if (MoveInput.IsNearlyZero()) return;

	// 카메라를 움직이면 추적 모드 해제
	bIsFollowingCharacter = false;

	const FRotator YawRotation(0.f, currentCameraYaw, 0.f);
	const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDir   = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	// 목표 속도를 향해 부드럽게 가속 (VInterpTo = 지수 감쇠 보간)
	const FVector TargetVelocity = (ForwardDir * MoveInput.Y + RightDir * MoveInput.X) * cameraMoveSpeed;
	cameraVelocity = FMath::VInterpTo(cameraVelocity, TargetVelocity, DeltaTime, cameraMoveSmoothing);

	SpringArm->AddWorldOffset(cameraVelocity * DeltaTime);
	SnapSpringArmToGround(SpringArm);

	bCameraInputActive = true;
}

// ─── 카메라 회전 ──────────────────────────────────────────────────────────────
void ABattleController::OnCameraRotate(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm) return;

	const float RotateInput = Value.Get<float>();
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	currentCameraYaw += RotateInput * cameraRotateSpeed * DeltaTime;

	// Pitch는 고정, Yaw만 변경
	SpringArm->SetRelativeRotation(FRotator(cachedSpringArmPitch, currentCameraYaw, 0.f));
}

// ─── 카메라 줌 ───────────────────────────────────────────────────────────────
void ABattleController::OnCameraZoom(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm) return;

	const float ZoomInput = Value.Get<float>();
	SpringArm->TargetArmLength = FMath::Clamp(
		SpringArm->TargetArmLength - ZoomInput * cameraZoomSpeed,
		cameraZoomMin,
		cameraZoomMax
	);
}

// ─── 이동 명령 (좌클릭) ──────────────────────────────────────────────────────
void ABattleController::OnMoveCommand(const FInputActionValue& Value)
{
	if (!activeUnit) return;

	// 스킬 모드 우선 처리
	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (SkillComp && SkillComp->IsSkillActive())
	{
		ExecuteSkill();
		return;
	}

	// 이동 모드
	if (!bIsMoveMode || !IsValid(cursorIndicatorInstance)) return;

	const TArray<FVector>& PathPoints = cursorIndicatorInstance->GetCachedPathPoints();
	if (PathPoints.Num() == 0) return;

	cursorIndicatorInstance->LockAtCurrentPosition();
	activeUnit->MoveAlongPath(PathPoints);
	UE_LOG(LogTemp, Log, TEXT("[BattleController] 이동 명령: %s (%d개 경유점)"),
		*PathPoints.Last().ToString(), PathPoints.Num());
}

// ─── 이동 취소 (우클릭) ──────────────────────────────────────────────────────
void ABattleController::OnCancelMove(const FInputActionValue& Value)
{
	// 스킬 모드 활성 시 스킬 취소 우선
	if (activeUnit)
	{
		USkillComponent* SkillComp = activeUnit->GetSkillComponent();
		if (SkillComp && SkillComp->IsSkillActive())
		{
			SkillComp->DeactivateSkill();
			return;
		}
	}

	if (!bIsMoveMode || !activeUnit) return;

	activeUnit->StopMovement();
	ExitMoveMode();
	UE_LOG(LogTemp, Log, TEXT("[BattleController] 이동 취소"));
}

// ─── 카메라 초기화 ───────────────────────────────────────────────────────────
void ABattleController::OnCameraReset(const FInputActionValue& Value)
{
	if (!cachedSpringArm || !activeUnit) return;

	// 스프링암을 즉시 캐릭터 위치로 이동 후 추적 모드 활성화
	cachedSpringArm->SetWorldLocation(activeUnit->GetActorLocation());
	SnapSpringArmToGround(cachedSpringArm);
	cameraVelocity = FVector::ZeroVector;
	bIsFollowingCharacter = true;

	// 카메라 각도를 초기값으로 리셋 (턴 전환 시 이전 캐릭터의 Yaw가 남는 문제 방지)
	currentCameraYaw = 0.f;
	cachedSpringArm->SetRelativeRotation(FRotator(cachedSpringArmPitch, currentCameraYaw, 0.f));
}

// ─── 스킬 모드 ──────────────────────────────────────────────────────────────
void ABattleController::ActivateSkill(UActiveSkillBase* Skill)
{
	if (!activeUnit || !Skill) return;

	// 이동 모드와 상호 배타
	if (bIsMoveMode) ExitMoveMode();

	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (SkillComp)
	{
		SkillComp->ActivateSkill(Skill);
	}
}

void ABattleController::DeactivateSkill()
{
	if (!activeUnit) return;

	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (SkillComp && SkillComp->IsSkillActive())
	{
		SkillComp->DeactivateSkill();
	}
}

// ─── 이동 모드 ───────────────────────────────────────────────────────────────
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

	// 스킬 모드와 상호 배타
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
	// 잠금 해제만으로 마우스 추적 재개 — 스폰/파괴 불필요
	if (IsValid(cursorIndicatorInstance))
	{
		cursorIndicatorInstance->Unlock();
	}
}

void ABattleController::OnUnitMovementCompleted()
{
	if (!bIsMoveMode || !activeUnit) return;

	if (activeUnit->GetCurrentMovingPoint() <= 0.f)
	{
		// 이동력 소진 → 이동 모드 자동 종료
		ExitMoveMode();
	}
	else
	{
		// 이동력 잔여 → 커서 인디케이터 리셋해 다음 이동 준비
		ResetCursorIndicator();
	}
}

// ─── 스킬 실행 ──────────────────────────────────────────────────────────────
void ABattleController::ExecuteSkill()
{
	USkillComponent* SkillComp = activeUnit->GetSkillComponent();
	if (!SkillComp) return;

	UActiveSkillBase* Skill = SkillComp->GetCurrentSkill();
	if (!Skill) return;

	// Self 모드가 아닌 경우 — 커서 아래 클릭 대상을 EPickTeam으로 검증
	if (Skill->selectMode == ESelectMode::SinglePick)
	{
		// ECC_Visibility 사용: 캐릭터 메시/캡슐이 Visibility를 Block함
		// (ECC_Pawn은 캡슐이 ECR_Overlap으로 응답해 바닥에 히트되므로 사용 불가)
		FHitResult HitResult;
		GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

		ACharacterBase* ClickedChar = Cast<ACharacterBase>(HitResult.GetActor());
		// 단일 대상 — 반드시 유효한 캐릭터를 클릭해야 함
		if (!IsValidSkillTarget(ClickedChar, Skill->pickTeam)) return;
	}

	// 스킬 영향 대상은 오버랩 단계에서 EAreaTarget으로 이미 필터된 SkillComponent 목록 사용
	const TArray<ACharacterBase*> Targets = SkillComp->GetCurrentTargets();
	if (Targets.Num() == 0 && Skill->selectMode != ESelectMode::Self) return;

	// 전투 예측 값은 이미 오버랩 시 CalculateDamage로 세팅됨 → 바로 ReflectDamage
	for (ACharacterBase* Target : Targets)
	{
		Target->ReflectDamage();
	}

	// 스킬 고유 로직 실행 (파생 클래스에서 오버라이드)
	Skill->Execute(Targets);

	// 스킬 사용 완료 → 비활성화
	SkillComp->DeactivateSkill();

	UE_LOG(LogTemp, Log, TEXT("[BattleController] 스킬 실행: %s → %d명 대상"),
		*Skill->skillName.ToString(), Targets.Num());
}

bool ABattleController::IsValidSkillTarget(ACharacterBase* Target, EPickTeam PickTeam) const
{
	if (!Target) return false;

	switch (PickTeam)
	{
	case EPickTeam::EnemyOnly:
		return Cast<AEnemyBase>(Target) != nullptr;
	case EPickTeam::AllyOnly:
		return Cast<AAllyCharacterBase>(Target) != nullptr;
	case EPickTeam::Any:
		return true;
	}
	return false;
}

// ─── 헬퍼: 지면 스냅 ─────────────────────────────────────────────────────────
void ABattleController::SnapSpringArmToGround(USpringArmComponent* SpringArm)
{
	const FVector PivotPos = SpringArm->GetComponentLocation();
	FHitResult GroundHit;
	if (GetWorld()->LineTraceSingleByChannel(
		GroundHit,
		PivotPos + FVector(0.f, 0.f, 500.f),
		PivotPos - FVector(0.f, 0.f, 500.f),
		ECC_Visibility))
	{
		SpringArm->SetWorldLocation(FVector(PivotPos.X, PivotPos.Y, GroundHit.Location.Z + cameraGroundOffset));
	}
}

