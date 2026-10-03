//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "SettingsWidget.generated.h"

class UButton;
class UCheckBox;
class UComboBoxString;
class UTextBlock;
class UPWGameUserSettings;

//설정 화면
//해상도·창모드만 적용 버튼을 거치고 나머지는 즉시 적용된다 — 잘못된 해상도는 되돌릴 UI가 안 보일 수 있으므로
UCLASS()
class PW_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//뒤로가기 시 호출, 복귀 방법은 여는 쪽이 정한다
	//허브는 화면 교체이고 전투 일시정지는 겹침 제거라 수명 관리가 달라 이쪽이 알 수 없다
	FSimpleDelegate onClosed;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//화면 — 적용 버튼을 거치는 둘
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeCombo;

	//화면 — 즉시 적용
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> VSyncCheck;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> FrameRateCombo;

	//품질 — 프리셋과 개별 6축, 전부 같은 5단계 척도를 쓴다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> OverallQualityCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ShadowQualityCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> TextureQualityCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> EffectQualityCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> PostProcessQualityCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> AntiAliasingQualityCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ViewDistanceQualityCombo;

	//게임플레이
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> EnemyTurnSpeedCombo;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> EnemyTurnCameraCheck;

	//해상도 적용과 되돌리기 확인
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> ConfirmPanel;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ConfirmButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ConfirmCountdownText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DefaultsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

	//해상도 적용 후 확인을 기다리는 시간, 넘기면 이전 모드로 되돌린다
	UPROPERTY(EditDefaultsOnly, Category = "Settings")
	int32 videoConfirmSeconds = 15;

private:
	//현재 설정값을 모든 컨트롤에 반영, 생성 시점과 기본값 복원 후에 호출
	void RefreshFromSettings();

	//품질 6축만 갱신, 프리셋을 바꾸면 개별 축이 따라 움직이므로 분리해 둔다
	void RefreshQualityCombos(const UPWGameUserSettings* settings);

	//5단계 척도를 채우고 현재 단계를 선택, 품질 콤보 7개가 공유한다
	void FillQualityOptions(UComboBoxString* combo, int32 level);

	//즉시 적용 항목의 공통 마무리
	void ApplyImmediate(UPWGameUserSettings* settings);

	//해상도 확인 패널 표시·숨김과 1초 카운트다운
	void ShowVideoConfirm();
	void HideVideoConfirm();
	void TickVideoConfirm();

	//확인 없이 시간이 다 가면 이전 해상도로 복구
	void RevertVideoMode();

	//ResolutionCombo 인덱스 → 실제 해상도, 문자열을 되파싱하지 않기 위해 들고 있는다
	TArray<FIntPoint> resolutionOptions;

	FTimerHandle videoConfirmTimerHandle;
	int32 videoConfirmRemaining = 0;

	UFUNCTION()
	void HandleVSyncChanged(bool bIsChecked);
	UFUNCTION()
	void HandleFrameRateChanged(FString selectedItem, ESelectInfo::Type selectInfo);

	UFUNCTION()
	void HandleOverallQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandleShadowQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandleTextureQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandleEffectQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandlePostProcessQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandleAntiAliasingQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandleViewDistanceQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);

	UFUNCTION()
	void HandleEnemyTurnSpeedChanged(FString selectedItem, ESelectInfo::Type selectInfo);
	UFUNCTION()
	void HandleEnemyTurnCameraChanged(bool bIsChecked);

	UFUNCTION()
	void HandleApplyClicked();
	UFUNCTION()
	void HandleConfirmClicked();
	UFUNCTION()
	void HandleDefaultsClicked();
	UFUNCTION()
	void HandleBackClicked();
};