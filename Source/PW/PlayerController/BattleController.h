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
class ACharacterBase;
class UBattleTurnWidget;
class UActiveSkillBase;
class AAttackRangeIndicator;

//전투 씬 플레이어 컨트롤러, EnhancedInput 기반 카메라 조작 및 유닛 이동 명령 처리
UCLASS()
class PW_API ABattleController : public APlayerController
{
	GENERATED_BODY()

public:
	ABattleController();

	//턴 시작 시 GameMode에서 호출
	void InitTurn(AAllyCharacterBase* TurnUnit);
	void EndTurn();

	//적 턴 시작 시 GameMode에서 호출, 카메라 폰 스프링암을 피벗으로 AI 추적
	void BeginAITurnFollow(ACharacterBase* AIUnit);

	//시야 밖 적 턴, 추적 대상만 해제하고 카메라는 무조작
	void ClearAITurnFollow() { aiFollowTarget = nullptr; }

	//MoveWidget 버튼에서 호출, 이동 모드 토글
	void ToggleMoveMode();

	//SkillButton에서 호출, 스킬 모드 진입/해제
	void ActivateSkill(UActiveSkillBase* Skill);
	void DeactivateSkill();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	
	//전투 씬 기본 입력 매핑 컨텍스트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> battleInputMappingContext;

	//카메라 상하좌우 이동
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> iA_CameraMove;

	//카메라 좌우 회전
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> iA_CameraRotate;

	//카메라 줌 인/아웃
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Camera")
	TObjectPtr<UInputAction> iA_CameraZoom;

	//유닛 이동 명령
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Selection")
	TObjectPtr<UInputAction> iA_MoveCommand;

	//이동 취소
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input|Selection")
	TObjectPtr<UInputAction> iA_CancelMove;

	//카메라 초기화
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UInputAction> iA_CameraReset;

	//카메라 이동 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraMoveSpeed = 1200.f;

	//카메라 회전 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraRotateSpeed = 90.f;

	//줌 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraZoomSpeed = 200.f;

	//최소 줌 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraZoomMin = 500.f;

	//최대 줌 거리
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraZoomMax = 2500.f;

	//지면 스냅 후 스프링암 피벗을 띄울 높이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraGroundOffset = 10.f;

	//카메라 이동 보간 속도
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraMoveSmoothing = 8.f;

private:
	void OnCameraMove(const FInputActionValue& Value);
	void OnCameraRotate(const FInputActionValue& Value);
	void OnCameraZoom(const FInputActionValue& Value);
	void OnMoveCommand(const FInputActionValue& Value);
	void OnCancelMove(const FInputActionValue& Value);
	void OnCameraReset(const FInputActionValue& Value);

	//현재 카메라가 바라보는 Yaw 각도, 회전 누적용
	float currentCameraYaw = 0.f;

	//상시 빙의 중인 카메라 폰의 스프링암 캐시
	UPROPERTY()
	TObjectPtr<class USpringArmComponent> cachedSpringArm = nullptr;

	//카메라 폰 스프링암 반환, 캐시 미보유 시 빙의 폰에서 재캐시
	class USpringArmComponent* ResolveSpringArm();

	//true면 Tick에서 스프링암 위치를 캐릭터 위치로 고정, false면 자유 이동 모드
	bool bIsFollowingCharacter = true;

	//Detach 후에도 Pitch를 복원하기 위해 초기값 캐싱
	float cachedSpringArmPitch = -55.f;

	//스프링암 피벗을 지면에 스냅, 경사면 관통 방지
	void SnapSpringArmToGround(USpringArmComponent* SpringArm);

	void EnterMoveMode();
	void ExitMoveMode();
	//이동 완료 후 커서 인디케이터 리셋
	void ResetCursorIndicator();

	//activeUnit->OnMovementCompleted 수신 핸들러
	void OnUnitMovementCompleted();

	//이동 모드 활성 여부, 커서 인디케이터 표시 및 이동/취소 입력 유효
	bool bIsMoveMode = false;

	//카메라 이동 속도, 매 프레임 Interp로 갱신
	FVector cameraVelocity = FVector::ZeroVector;

	//이번 프레임에 카메라 이동 입력이 있었는지 여부, 감속 판단용
	bool bCameraInputActive = false;

	//커서 인디케이터 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ACursorIndicator> cursorIndicatorClass;

	//스폰된 인디케이터 인스턴스
	UPROPERTY()
	TObjectPtr<ACursorIndicator> cursorIndicatorInstance = nullptr;

	//턴 HUD 위젯 클래스, 이동/스킬/턴종료/게이지 일괄 포함
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBattleTurnWidget> turnHudWidgetClass;

	//생성된 턴 HUD 인스턴스
	UPROPERTY()
	TObjectPtr<UBattleTurnWidget> turnHudWidgetInstance = nullptr;

	//activeUnit->OnVitalsChanged 수신, 게이지 수치 갱신
	void RefreshGauges();

	//스킬 실행, 타겟 검증 후 ReflectDamage + Execute
	void ExecuteSkill();

	//스킬 버튼 활성/비활성 상태 갱신
	void RefreshSkillButtons();

	//사거리 밖 자동이동 후 스킬 실행 대기 플래그
	bool bPendingSkillExec = false;

	//멀티픽(pickCount > 1) 잔여 선택 횟수
	int32 remainingPicks = 0;

	UPROPERTY()
	TObjectPtr<AAllyCharacterBase> activeUnit = nullptr;

	//적 턴에 카메라가 추적할 AI 유닛, activeUnit이 null인 동안만 유효
	UPROPERTY()
	TObjectPtr<ACharacterBase> aiFollowTarget = nullptr;
};
