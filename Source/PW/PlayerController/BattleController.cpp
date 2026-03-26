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
	activeUnit->InitTurn();
	
	// 커서 인디케이터 스폰
	if (cursorIndicatorClass)
	{
		cursorIndicatorInstance = GetWorld()->SpawnActor<ACursorIndicator>(cursorIndicatorClass);
		if (cursorIndicatorInstance)
		{
			cursorIndicatorInstance->SetActiveUnit(TurnUnit);
		}
	}

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
	// CharacterBase의 Tick 이동 상태(bIsMovingToTarget)까지 초기화
	if (IsValid(activeUnit))
	{
		activeUnit->EndTurn();
	}
	// 기존 턴 종료 위젯 제거
	if (IsValid(turnEndWidgetInstance))
	{
		turnEndWidgetInstance->RemoveFromParent();
		turnEndWidgetInstance = nullptr;
	}
	// 기존 인디케이터 정리
	if (IsValid(cursorIndicatorInstance))
	{
		cursorIndicatorInstance->Destroy();
		cursorIndicatorInstance = nullptr;
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

// ─── Tick: 감속 및 지면 스냅 ─────────────────────────────────────────────────
void ABattleController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 입력이 없는 프레임: 속도를 0으로 감속하며 남은 관성 적용
	if (!bCameraInputActive && !cameraVelocity.IsNearlyZero(1.f))
	{
		cameraVelocity = FMath::VInterpTo(cameraVelocity, FVector::ZeroVector, DeltaTime, cameraMoveSmoothing);

		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn)
		{
			if (USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>())
			{
				SpringArm->AddWorldOffset(cameraVelocity * DeltaTime);
				SnapSpringArmToGround(SpringArm);
			}
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

	// 카메라를 처음 움직일 때 캐릭터로부터 분리
	if (bIsAttached)
	{
		DetachCameraFromCharacter(SpringArm);
	}

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
	if (!activeUnit || !IsValid(cursorIndicatorInstance)) return;

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
	if (AAllyCharacterBase* Unit = Cast<AAllyCharacterBase>(GetPawn()))
	{
		Unit->StopMovement();
		UE_LOG(LogTemp, Log, TEXT("[BattleController] 이동 취소"));
	}
}

// ─── 카메라 초기화 ───────────────────────────────────────────────────────────
void ABattleController::OnCameraReset(const FInputActionValue& Value)
{
	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	USpringArmComponent* SpringArm = ControlledPawn->FindComponentByClass<USpringArmComponent>();
	if (!SpringArm) return;

	AttachCameraToCharacter(SpringArm, ControlledPawn);
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

// ─── 헬퍼: 카메라 분리 ───────────────────────────────────────────────────────
void ABattleController::DetachCameraFromCharacter(USpringArmComponent* SpringArm)
{
	// 현재 world transform을 유지한 채 분리 → 카메라 위치 고정
	SpringArm->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	bIsAttached = false;
}

// ─── 헬퍼: 카메라 재부착 ─────────────────────────────────────────────────────
void ABattleController::AttachCameraToCharacter(USpringArmComponent* SpringArm, APawn* OwnerPawn)
{
	// 캐릭터 루트에 스냅 후 Pitch/Yaw 복원
	SpringArm->AttachToComponent(
		OwnerPawn->GetRootComponent(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale
	);
	// SnapToTarget은 relative rotation을 초기화하므로 Pitch와 Yaw를 다시 설정
	SpringArm->SetRelativeRotation(FRotator(cachedSpringArmPitch, currentCameraYaw, 0.f));
	bIsAttached = true;
}
