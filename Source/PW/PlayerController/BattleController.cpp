// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Characters/CharacterBase.h"
#include "Actors/CursorIndicator.h"

ABattleController::ABattleController()
{
	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
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

void ABattleController::InitTurn(ACharacterBase* TurnUnit)
{
	// 기존 인디케이터 정리
	if (IsValid(cursorIndicatorInstance))
	{
		cursorIndicatorInstance->Destroy();
		cursorIndicatorInstance = nullptr;
	}

	if (cursorIndicatorClass && IsValid(TurnUnit))
	{
		cursorIndicatorInstance = GetWorld()->SpawnActor<ACursorIndicator>(cursorIndicatorClass);
		if (cursorIndicatorInstance)
		{
			cursorIndicatorInstance->SetActiveUnit(TurnUnit);
		}
	}
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
	const FVector Delta = (ForwardDir * MoveInput.Y + RightDir * MoveInput.X) * cameraMoveSpeed * DeltaTime;
	SpringArm->AddWorldOffset(Delta);
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
	FHitResult HitResult;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	if (!bHit) return;

	if (ACharacterBase* Unit = Cast<ACharacterBase>(GetPawn()))
	{
		Unit->MoveToLocation(HitResult.Location);
		UE_LOG(LogTemp, Log, TEXT("[BattleController] 이동 명령: %s"), *HitResult.Location.ToString());
	}
}

// ─── 이동 취소 (우클릭) ──────────────────────────────────────────────────────
void ABattleController::OnCancelMove(const FInputActionValue& Value)
{
	if (ACharacterBase* Unit = Cast<ACharacterBase>(GetPawn()))
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
