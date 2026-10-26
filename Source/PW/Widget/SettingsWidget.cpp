//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SettingsWidget.h"
#include "Settings/PWGameUserSettings.h"
#include "GameInstance/PWGameInstance.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/ProgressBar.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"

namespace
{
	//품질 5단계, 인덱스가 곧 scalability level이다
	const TArray<FString>& QualityLabels()
	{
		static const TArray<FString> labels = { TEXT("낮음"), TEXT("중간"), TEXT("높음"), TEXT("에픽"), TEXT("시네마틱") };
		return labels;
	}

	//프레임 상한 후보, 0은 무제한
	const TArray<float>& FrameRateOptions()
	{
		static const TArray<float> options = { 30.f, 60.f, 120.f, 144.f, 0.f };
		return options;
	}

	//창 모드 후보, 순서는 EWindowMode 값과 무관하게 ConvertIntToWindowMode로 변환한다
	const TArray<FString>& WindowModeLabels()
	{
		static const TArray<FString> labels = { TEXT("전체화면"), TEXT("전체화면 창"), TEXT("창") };
		return labels;
	}
}

void USettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (OverallQualityCombo) OverallQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleOverallQualityChanged);

	if (MasterVolumeSlider) MasterVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::HandleMasterVolumeChanged);
	if (MusicVolumeSlider)  MusicVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::HandleMusicVolumeChanged);
	if (SfxVolumeSlider)    SfxVolumeSlider->OnValueChanged.AddDynamic(this, &USettingsWidget::HandleSfxVolumeChanged);

	if (ApplyButton)    ApplyButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleApplyClicked);
	if (DefaultsButton) DefaultsButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleDefaultsClicked);
	if (BackButton)     BackButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleBackClicked);

	RefreshFromSettings(UPWGameUserSettings::Get());
}

void USettingsWidget::RefreshFromSettings(const UPWGameUserSettings* source)
{
	if (!source) return;

	//해상도 — 모니터가 보고한 모드만 올린다
	if (ResolutionCombo)
	{
		resolutionOptions.Reset();
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(resolutionOptions);

		//기본값은 0x0(데스크톱 해상도 사용)이라 실제 크기로 바꿔 표시한다
		FIntPoint current = source->GetScreenResolution();
		if (current.X <= 0 || current.Y <= 0)
		{
			current = source->GetDesktopResolution();
		}

		//현재 해상도가 목록에 없을 수 있다(창 모드에서 임의 크기), 그때도 고를 수 있게 끼워 넣는다
		if (!resolutionOptions.Contains(current))
		{
			resolutionOptions.Add(current);
		}

		ResolutionCombo->ClearOptions();
		for (const FIntPoint& res : resolutionOptions)
		{
			ResolutionCombo->AddOption(FString::Printf(TEXT("%d x %d"), res.X, res.Y));
		}
		ResolutionCombo->SetSelectedIndex(resolutionOptions.IndexOfByKey(current));
	}

	if (WindowModeCombo)
	{
		WindowModeCombo->ClearOptions();
		for (const FString& label : WindowModeLabels())
		{
			WindowModeCombo->AddOption(label);
		}
		WindowModeCombo->SetSelectedIndex(static_cast<int32>(source->GetFullscreenMode()));
	}

	if (VSyncCheck)
	{
		VSyncCheck->SetIsChecked(source->IsVSyncEnabled());
	}

	if (FrameRateCombo)
	{
		FrameRateCombo->ClearOptions();
		for (const float limit : FrameRateOptions())
		{
			FrameRateCombo->AddOption(limit > 0.f ? FString::Printf(TEXT("%d"), FMath::RoundToInt(limit)) : TEXT("무제한"));
		}

		//저장된 상한이 후보에 없으면 무제한(마지막 항목)으로 표시한다
		const int32 index = FrameRateOptions().IndexOfByKey(source->GetFrameRateLimit());
		FrameRateCombo->SetSelectedIndex(index != INDEX_NONE ? index : FrameRateOptions().Num() - 1);
	}

	FillQualityOptions(OverallQualityCombo, source->GetOverallScalabilityLevel());
	FillQualityOptions(ShadowQualityCombo, source->GetShadowQuality());
	FillQualityOptions(TextureQualityCombo, source->GetTextureQuality());
	FillQualityOptions(EffectQualityCombo, source->GetVisualEffectQuality());
	FillQualityOptions(PostProcessQualityCombo, source->GetPostProcessingQuality());
	FillQualityOptions(AntiAliasingQualityCombo, source->GetAntiAliasingQuality());
	FillQualityOptions(ViewDistanceQualityCombo, source->GetViewDistanceQuality());

	SetVolumeDisplay(MasterVolumeSlider, MasterVolumeBar, MasterVolumeText, source->GetMasterVolume());
	SetVolumeDisplay(MusicVolumeSlider, MusicVolumeBar, MusicVolumeText, source->GetMusicVolume());
	SetVolumeDisplay(SfxVolumeSlider, SfxVolumeBar, SfxVolumeText, source->GetSfxVolume());

	if (EnemyTurnSpeedCombo)
	{
		const TArray<float>& speeds = UPWGameUserSettings::GetEnemyTurnSpeedOptions();
		EnemyTurnSpeedCombo->ClearOptions();
		for (const float speed : speeds)
		{
			EnemyTurnSpeedCombo->AddOption(FString::Printf(TEXT("%gx"), speed));
		}

		//ini를 손편집해 후보에 없는 값이 들어와도 선택이 비지 않도록 1x로 떨어뜨린다
		const int32 index = speeds.IndexOfByKey(source->GetEnemyTurnSpeed());
		EnemyTurnSpeedCombo->SetSelectedIndex(index != INDEX_NONE ? index : 0);
	}

	if (EnemyTurnCameraCheck)
	{
		EnemyTurnCameraCheck->SetIsChecked(source->IsEnemyTurnCameraFollowEnabled());
	}
}

void USettingsWidget::FillQualityOptions(UComboBoxString* combo, int32 level)
{
	if (!combo) return;

	combo->ClearOptions();
	for (const FString& label : QualityLabels())
	{
		combo->AddOption(label);
	}

	//GetOverallScalabilityLevel은 축이 서로 다르면 -1을 돌려준다, 그때는 선택을 비워 둔다
	if (QualityLabels().IsValidIndex(level))
	{
		combo->SetSelectedIndex(level);
	}
	else
	{
		combo->ClearSelection();
	}
}

void USettingsWidget::SetVolumeDisplay(USlider* slider, UProgressBar* bar, UTextBlock* text, float volume)
{
	if (slider) slider->SetValue(volume);
	if (bar)    bar->SetPercent(volume);
	if (text)   text->SetText(FText::FromString(FString::Printf(TEXT("%d%%"), FMath::RoundToInt(volume * 100.f))));
}

void USettingsWidget::HandleOverallQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	//코드가 선택을 바꿀 때도 Direct로 불린다, 표시 갱신이 개별 축을 덮지 않도록 걸러낸다
	if (selectInfo == ESelectInfo::Direct) return;

	const int32 level = OverallQualityCombo->GetSelectedIndex();
	if (!QualityLabels().IsValidIndex(level)) return;

	//프리셋은 개별 축을 통째로 정하므로 화면만 따라 맞춘다
	for (UComboBoxString* combo : { ShadowQualityCombo.Get(), TextureQualityCombo.Get(), EffectQualityCombo.Get(),
		PostProcessQualityCombo.Get(), AntiAliasingQualityCombo.Get(), ViewDistanceQualityCombo.Get() })
	{
		if (combo) combo->SetSelectedIndex(level);
	}
}

void USettingsWidget::HandleMasterVolumeChanged(float value)
{
	SetVolumeDisplay(MasterVolumeSlider, MasterVolumeBar, MasterVolumeText, value);
}

void USettingsWidget::HandleMusicVolumeChanged(float value)
{
	SetVolumeDisplay(MusicVolumeSlider, MusicVolumeBar, MusicVolumeText, value);
}

void USettingsWidget::HandleSfxVolumeChanged(float value)
{
	SetVolumeDisplay(SfxVolumeSlider, SfxVolumeBar, SfxVolumeText, value);
}

void USettingsWidget::HandleApplyClicked()
{
	UPWGameUserSettings* settings = UPWGameUserSettings::Get();
	if (!settings) return;

	const int32 resIndex = ResolutionCombo ? ResolutionCombo->GetSelectedIndex() : INDEX_NONE;
	if (resolutionOptions.IsValidIndex(resIndex))
	{
		settings->SetScreenResolution(resolutionOptions[resIndex]);
	}

	const int32 windowIndex = WindowModeCombo ? WindowModeCombo->GetSelectedIndex() : INDEX_NONE;
	if (WindowModeLabels().IsValidIndex(windowIndex))
	{
		settings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(windowIndex));
	}

	if (VSyncCheck)
	{
		settings->SetVSyncEnabled(VSyncCheck->IsChecked());
	}

	const int32 frameIndex = FrameRateCombo ? FrameRateCombo->GetSelectedIndex() : INDEX_NONE;
	if (FrameRateOptions().IsValidIndex(frameIndex))
	{
		settings->SetFrameRateLimit(FrameRateOptions()[frameIndex]);
	}

	//빈 선택(-1)은 건너뛴다, 축이 섞여 프리셋이 비어 있을 때 개별 축만 반영하기 위함
	const auto applyQuality = [settings](const UComboBoxString* combo, void (UGameUserSettings::*setter)(int32))
	{
		const int32 level = combo ? combo->GetSelectedIndex() : INDEX_NONE;
		if (QualityLabels().IsValidIndex(level))
		{
			(settings->*setter)(level);
		}
	};

	//프리셋을 먼저 걸어 개별 축에 없는 항목(GI·반사 등)까지 맞춘 뒤 개별 축으로 덮는다
	applyQuality(OverallQualityCombo, &UGameUserSettings::SetOverallScalabilityLevel);
	applyQuality(ShadowQualityCombo, &UGameUserSettings::SetShadowQuality);
	applyQuality(TextureQualityCombo, &UGameUserSettings::SetTextureQuality);
	applyQuality(EffectQualityCombo, &UGameUserSettings::SetVisualEffectQuality);
	applyQuality(PostProcessQualityCombo, &UGameUserSettings::SetPostProcessingQuality);
	applyQuality(AntiAliasingQualityCombo, &UGameUserSettings::SetAntiAliasingQuality);
	applyQuality(ViewDistanceQualityCombo, &UGameUserSettings::SetViewDistanceQuality);

	if (MasterVolumeSlider) settings->SetMasterVolume(MasterVolumeSlider->GetValue());
	if (MusicVolumeSlider)  settings->SetMusicVolume(MusicVolumeSlider->GetValue());
	if (SfxVolumeSlider)    settings->SetSfxVolume(SfxVolumeSlider->GetValue());

	const TArray<float>& speeds = UPWGameUserSettings::GetEnemyTurnSpeedOptions();
	const int32 speedIndex = EnemyTurnSpeedCombo ? EnemyTurnSpeedCombo->GetSelectedIndex() : INDEX_NONE;
	if (speeds.IsValidIndex(speedIndex))
	{
		settings->SetEnemyTurnSpeed(speeds[speedIndex]);
	}

	if (EnemyTurnCameraCheck)
	{
		settings->SetEnemyTurnCameraFollowEnabled(EnemyTurnCameraCheck->IsChecked());
	}

	//확인 대기 없이 확정한다, ApplySettings가 저장까지 하므로 그 전에 확정값을 맞춘다
	settings->ConfirmVideoMode();
	settings->ApplySettings(false);

	if (UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>())
	{
		gameInstance->ApplyAudioSettings();
	}

	//축이 섞이면 프리셋 표시가 비는 등 실제 결과를 다시 보여준다
	RefreshFromSettings(settings);
}

void USettingsWidget::HandleDefaultsClicked()
{
	//실제 설정은 적용 전까지 건드리지 않는다, 기본값만 담은 임시 객체로 화면을 채운다
	UPWGameUserSettings* defaults = NewObject<UPWGameUserSettings>();
	defaults->SetToDefaults();
	RefreshFromSettings(defaults);
}

void USettingsWidget::HandleBackClicked()
{
	//여는 쪽이 이 위젯을 제거할 수 있으므로 Execute 직후 아무것도 하지 않고 빠진다
	onClosed.ExecuteIfBound();
}
