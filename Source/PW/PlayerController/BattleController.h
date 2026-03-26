// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class ACursorIndicator;
class AAllyCharacterBase;
class UTurnEndWidget;

// 전투 씬 플레이어 컨트롤러
// EnhancedInput 기반 카메라 조작 및 유닛 이동 명령 처리
UCLASS()
class PW_API ABattleController : public APlayerController
{
	GENERATED_BODY()

public:
	ABattleController();

	// 턴 시작 시 호출 - GameMode에서 호출
	void InitTurn(AAllyCharacterBase* TurnUnit);
	void EndTurn();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;

	// ─── 입력 매핑 컨텍스트 ───────────────────────────────────

	// 전투 씬 기본 입력 매핑 컨텍스트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> battleInputMappingContext;

	// ─── 입력 액션 ───────────────────────────────────────────

	// 카메라 상하좌우 이동
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> iA_CameraMove;

	// 카메라 좌우 회전
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> iA_CameraRotate;

	// 카메라 줌 인/아웃
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> iA_CameraZoom;

	// 유닛 이동 명령
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Selection")
	TObjectPtr<UInputAction> iA_MoveCommand;

	// 이동 취소
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Selection")
	TObjectPtr<UInputAction> iA_CancelMove;

	// 카메라 초기화
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UInputAction> iA_CameraReset;

	// ─── 카메라 설정 ─────────────────────────────────────────

	// 카메라 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraMoveSpeed = 1200.f;

	// 카메라 회전 속도 (도/초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraRotateSpeed = 90.f;

	// 줌 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraZoomSpeed = 200.f;

	// 최소 줌 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraZoomMin = 500.f;

	// 최대 줌 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraZoomMax = 2500.f;

	// 지면 스냅 후 스프링암 피벗을 띄울 높이 (cm)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraGroundOffset = 10.f;

	// 카메라 이동 보간 속도 (높을수록 반응이 빠름, 낮을수록 부드럽게 가속/감속)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraMoveSmoothing = 8.f;

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
	float currentCameraYaw = 0.f;

	// true: 스프링암이 캐릭터 루트에 attached → 캐릭터 이동을 따라감
	// false: 스프링암이 world에 독립 → 카메라 위치 고정
	bool bIsAttached = true;

	// Detach 후에도 Pitch를 복원하기 위해 초기값 캐싱
	float cachedSpringArmPitch = -55.f;

	// 스프링암 Attach/Detach 헬퍼
	void AttachCameraToCharacter(class USpringArmComponent* SpringArm, APawn* OwnerPawn);
	void DetachCameraFromCharacter(class USpringArmComponent* SpringArm);

	// 스프링암 피벗을 지면에 스냅 (경사면 관통 방지)
	void SnapSpringArmToGround(class USpringArmComponent* SpringArm);

	// 카메라 이동 속도 (월드 공간, 매 프레임 Interp로 갱신)
	FVector cameraVelocity = FVector::ZeroVector;

	// 이번 프레임에 카메라 이동 입력이 있었는지 여부 (감속 판단용)
	bool bCameraInputActive = false;

	// ─── 커서 인디케이터 ─────────────────────────────────────

	// 커서 인디케이터 클래스 (BP_CursorIndicator 지정)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ACursorIndicator> cursorIndicatorClass;

	// 스폰된 인디케이터 인스턴스 (GC 방지)
	UPROPERTY()
	TObjectPtr<ACursorIndicator> cursorIndicatorInstance = nullptr;

	// ─── 턴 종료 위젯 ────────────────────────────────────────

	// 턴 종료 버튼 위젯 클래스 (BP에서 WBP_TurnEnd 지정)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UTurnEndWidget> turnEndWidgetClass;

	// 생성된 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UTurnEndWidget> turnEndWidgetInstance = nullptr;

	UPROPERTY()
	TObjectPtr<AAllyCharacterBase> activeUnit = nullptr;
};
