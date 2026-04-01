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
}

void ABattleController::EndTurn()
{
	if (IsValid(activeUnit))
	{
		// 델리게이트 해제 후 이동 종료 (해제 먼저 해야 OnUnitMovementCompleted 오발 방지)
		activeUnit->OnMovementCompleted.RemoveAll(this);
		ExitMoveMode();
		activeUnit->EndTurn();
	}
	if (IsValid(turnEndWidgetInstance))
	{
		turnEndWidgetInstance->RemoveFromParent();
		turnEndWidgetInstance = nullptr;
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
	if (!bIsMoveMode || !activeUnit || !IsValid(cursorIndicatorInstance)) return;

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
	if (!bIsMoveMode || !activeUnit) return;

	activeUnit->StopMovement();
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
}

// ─── 이동 모드 ───────────────────────────────────────────────────────────────
void ABattleController::ToggleMoveMode()
{
	if (bIsMoveMode) ExitMoveMode();
	else             EnterMoveMode();
}

void ABattleController::EnterMoveMode()
{
	if (bIsMoveMode || !activeUnit || !cursorIndicatorClass) return;

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

