//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/SlateEnums.h"
#include "SettingsWidget.generated.h"

class UButton;
class UCheckBox;
class UComboBoxString;
class UProgressBar;
class USlider;
class UTextBlock;
class UPWGameUserSettings;

//설정 화면
//컨트롤 조작은 화면에만 머물고 적용 버튼이 전부 한 번에 반영한다
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

	//화면
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ResolutionCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> WindowModeCombo;
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

	//오디오 — 슬라이더는 손잡이만, 채움은 뒤에 깔린 프로그레스바가 그린다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MasterVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> MasterVolumeBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MasterVolumeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MusicVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> MusicVolumeBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MusicVolumeText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SfxVolumeSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> SfxVolumeBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SfxVolumeText;

	//게임플레이
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> EnemyTurnSpeedCombo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCheckBox> EnemyTurnCameraCheck;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ApplyButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> DefaultsButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> BackButton;

private:
	//source의 값을 모든 컨트롤에 표시, 열 때·적용 후엔 실제 설정, 초기화 땐 기본값 객체를 넘긴다
	void RefreshFromSettings(const UPWGameUserSettings* source);

	//5단계 척도를 채우고 현재 단계를 선택, 품질 콤보 7개가 공유한다
	void FillQualityOptions(UComboBoxString* combo, int32 level);

	//슬라이더 값을 채움과 퍼센트 글자에 맞춘다, SetValue는 OnValueChanged를 쏘지 않아 표시 때도 직접 부른다
	void SetVolumeDisplay(USlider* slider, UProgressBar* bar, UTextBlock* text, float volume);

	//ResolutionCombo 인덱스 → 실제 해상도, 문자열을 되파싱하지 않기 위해 들고 있는다
	TArray<FIntPoint> resolutionOptions;

	UFUNCTION()
	void HandleOverallQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo);

	UFUNCTION()
	void HandleMasterVolumeChanged(float value);
	UFUNCTION()
	void HandleMusicVolumeChanged(float value);
	UFUNCTION()
	void HandleSfxVolumeChanged(float value);

	UFUNCTION()
	void HandleApplyClicked();
	UFUNCTION()
	void HandleDefaultsClicked();
	UFUNCTION()
	void HandleBackClicked();
};
