// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class ACursorIndicator;
class ACharacterBase;

/**
 * 전투 씬 플레이어 컨트롤러
 * EnhancedInput 기반 카메라 조작 및 유닛 이동 명령 처리
 */
UCLASS()
class PW_API ABattleController : public APlayerController
{
	GENERATED_BODY()

public:
	ABattleController();

	/** 턴 시작 시 호출 - 추후 GameMode에서 호출 예정 */
	void InitTurn(ACharacterBase* TurnUnit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	// ─── 입력 매핑 컨텍스트 ───────────────────────────────────

	/** 전투 씬 기본 입력 매핑 컨텍스트 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> BattleInputMappingContext;

	// ─── 입력 액션 ───────────────────────────────────────────

	/** 카메라 상하좌우 이동 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> IA_CameraMove;

	/** 카메라 좌우 회전 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> IA_CameraRotate;

	/** 카메라 줌 인/아웃 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> IA_CameraZoom;

	/** 유닛 이동 명령 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Selection")
	TObjectPtr<UInputAction> IA_MoveCommand;

	/** 이동 취소 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Selection")
	TObjectPtr<UInputAction> IA_CancelMove;
	
	/** 카메라 초기화 **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UInputAction> IA_CameraReset;

	// ─── 카메라 설정 ─────────────────────────────────────────

	/** 카메라 이동 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraMoveSpeed = 1200.f;

	/** 카메라 회전 속도 (도/초) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraRotateSpeed = 90.f;

	/** 줌 속도 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraZoomSpeed = 200.f;

	/** 최소 줌 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraZoomMin = 500.f;

	/** 최대 줌 거리 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float CameraZoomMax = 2500.f;

private:
	// ─── 입력 콜백 ───────────────────────────────────────────

	void OnCameraMove(const FInputActionValue& Value);
	void OnCameraRotate(const FInputActionValue& Value);
	void OnCameraZoom(const FInputActionValue& Value);
	void OnMoveCommand(const FInputActionValue& Value);
	void OnCancelMove(const FInputActionValue& Value);
	void OnCameraReset(const FInputActionValue& Value);

	// ─── 내부 상태 ───────────────────────────────────────────

	// 현재 카메라가 바라보는 Yaw 각도 (회전 누적용)
	float CurrentCameraYaw = 0.f;

	// true: 스프링암이 캐릭터 루트에 attached → 캐릭터 이동을 따라감
	// false: 스프링암이 world에 독립 → 카메라 위치 고정
	bool bIsAttached = true;

	// Detach 후에도 Pitch를 복원하기 위해 초기값 캐싱
	float CachedSpringArmPitch = -55.f;

	// 스프링암 Attach/Detach 헬퍼
	void AttachCameraToCharacter(class USpringArmComponent* SpringArm, APawn* OwnerPawn);
	void DetachCameraFromCharacter(class USpringArmComponent* SpringArm);

	// ─── 커서 인디케이터 ─────────────────────────────────────

	/** 커서 인디케이터 클래스 (BP_CursorIndicator 지정) */
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ACursorIndicator> CursorIndicatorClass;

	// 스폰된 인디케이터 인스턴스 (GC 방지)
	UPROPERTY()
	TObjectPtr<ACursorIndicator> CursorIndicatorInstance = nullptr;
};