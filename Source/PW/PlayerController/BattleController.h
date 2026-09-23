// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameMode/BattleGameMode.h"
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
class UUpgradeSelectWidget;
class USkillBase;
class UDebugWidget;
class USkillComponent;
class UDeployWidget;
class UBattleResultWidget;
class UStagePreviewWidget;
class UCampRecruitWidget;

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

	//배치 확정, DeployWidget에서 호출. 전투로 넘어가면 배치 UI를 거둔다
	void ConfirmDeployment();

	//결과 화면의 버튼에서 호출, 스테이지를 진행시키고 예고 화면을 연다
	//패배나 완주면 예고 없이 허브로 돌아간다
	void LeaveBattle(EBattleResult result);

	//예고 화면의 버튼에서 호출, 예고한 스테이지로 실제 이동한다
	void TravelToNextStage();

	//예고 화면의 야영지 버튼, 끼워 넣었으면 true
	bool InsertCampVisit();

	//증원 강화가 호출, 직업 선택은 강화 선택이 모두 끝난 뒤로 미룬다
	void RequestReinforcement(AAllyCharacterBase* caller, int32 count);

	//직업 선택 화면이 호출, 신병 한 명을 합류시킨다. 남은 인원이 없으면 턴을 이어간다
	void ChooseReinforcementJob(int32 jobIndex);

	//적 턴 시작 시 GameMode에서 호출, 카메라 폰 스프링암을 피벗으로 AI 추적
	void BeginAITurnFollow(ACharacterBase* AIUnit);

	//시야 밖 적 턴, 추적 대상만 해제하고 카메라는 무조작
	void ClearAITurnFollow() { aiFollowTarget = nullptr; }

	//MoveWidget 버튼에서 호출, 이동 모드 토글
	void ToggleMoveMode();

	//SkillButton에서 호출, 스킬 모드 진입/해제
	void ActivateSkill(UActiveSkillBase* Skill);
	void DeactivateSkill();

	//디버그: 현재 턴 캐릭터 스트레스 +100
	void DebugAddStress();
	//디버그: 현재 턴 캐릭터 레벨 +1
	void DebugLevelUp();

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

	//레벨업 강화 선택 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUpgradeSelectWidget> upgradeSelectWidgetClass;

	//생성된 강화 선택 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UUpgradeSelectWidget> upgradeSelectWidgetInstance = nullptr;

	//디버그 조작 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDebugWidget> debugWidgetClass;

	//생성된 디버그 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UDebugWidget> debugWidgetInstance = nullptr;

	//출격 배치 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UDeployWidget> deployWidgetClass;

	//생성된 배치 위젯 인스턴스, 전투 개시 시 제거
	UPROPERTY()
	TObjectPtr<UDeployWidget> deployWidgetInstance = nullptr;

	//배치 시 지면에서 띄울 높이, 캡슐이 지면에 박히지 않게 하고 중력으로 안착시킨다
	UPROPERTY(EditDefaultsOnly, Category = "Deploy")
	float deploySpawnZOffset = 100.f;

	//전투 결과 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBattleResultWidget> resultWidgetClass;

	//생성된 결과 위젯 인스턴스
	UPROPERTY()
	TObjectPtr<UBattleResultWidget> resultWidgetInstance = nullptr;

	//다음 스테이지 예고 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStagePreviewWidget> previewWidgetClass;

	UPROPERTY()
	TObjectPtr<UStagePreviewWidget> previewWidgetInstance = nullptr;

	//증원 직업 선택 화면, 야영지 충원과 같은 위젯
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCampRecruitWidget> recruitWidgetClass;

	UPROPERTY()
	TObjectPtr<UCampRecruitWidget> recruitWidgetInstance = nullptr;

	//증원을 요청한 아군, 신병이 설 기준 위치이자 요청 대기 표시
	UPROPERTY()
	TObjectPtr<AAllyCharacterBase> reinforcementCaller = nullptr;

	//아직 직업을 고르지 않은 증원 인원
	int32 remainingReinforcements = 0;

	//대기 중인 증원 요청이 있으면 직업 선택을 연다, 없으면 HUD 잠금을 푼다
	void ShowReinforcementSelect();

	//증원 흐름 종료, 위젯을 거두고 요청자의 턴을 이어간다
	void FinishReinforcement();

	//Deploy 페이즈 진입 통지 수신, 배치 위젯 생성
	void OnDeployPhaseStarted();

	//Deploy 중 클릭 처리, 커서 아래 지점에 선택된 인원을 배치
	void HandleDeployClick();

	//전투 종료 통지 수신, 결과 위젯 생성
	void OnBattleFinished(EBattleResult result);

	//대기 중인 강화 선택을 하나 표시, 후보는 UUpgradeLibrary에서 추첨
	void ShowUpgradeSelect();

	//강화 선택 완료 핸들러, 습득 후 남은 큐가 있으면 이어서 표시
	void OnUpgradeChosen(TSubclassOf<USkillBase> Chosen);

	//activeUnit->OnVitalsChanged 수신, 게이지 수치 갱신
	UFUNCTION()
	void RefreshGauges();

	//스킬 실행 확정, 타겟 검증·비용 차감 후 효과를 pending 등록 (적용은 SkillImpact 노티파이 커밋 시점)
	void ExecuteSkill();

	//대상 루프(ReflectDamage + Execute)를 약참조 스냅샷으로 pending 등록
	void StartPendingSkillOnTargets(USkillComponent* SkillComp, UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets);

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
