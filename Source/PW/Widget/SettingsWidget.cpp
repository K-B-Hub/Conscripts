//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SettingsWidget.h"
#include "Settings/PWGameUserSettings.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/ComboBoxString.h"
#include "Components/TextBlock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

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

	if (VSyncCheck)                VSyncCheck->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::HandleVSyncChanged);
	if (FrameRateCombo)            FrameRateCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleFrameRateChanged);

	if (OverallQualityCombo)       OverallQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleOverallQualityChanged);
	if (ShadowQualityCombo)        ShadowQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleShadowQualityChanged);
	if (TextureQualityCombo)       TextureQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleTextureQualityChanged);
	if (EffectQualityCombo)        EffectQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleEffectQualityChanged);
	if (PostProcessQualityCombo)   PostProcessQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandlePostProcessQualityChanged);
	if (AntiAliasingQualityCombo)  AntiAliasingQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleAntiAliasingQualityChanged);
	if (ViewDistanceQualityCombo)  ViewDistanceQualityCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleViewDistanceQualityChanged);

	if (EnemyTurnSpeedCombo)       EnemyTurnSpeedCombo->OnSelectionChanged.AddDynamic(this, &USettingsWidget::HandleEnemyTurnSpeedChanged);
	if (EnemyTurnCameraCheck)      EnemyTurnCameraCheck->OnCheckStateChanged.AddDynamic(this, &USettingsWidget::HandleEnemyTurnCameraChanged);

	if (ApplyButton)    ApplyButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleApplyClicked);
	if (ConfirmButton)  ConfirmButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleConfirmClicked);
	if (DefaultsButton) DefaultsButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleDefaultsClicked);
	if (BackButton)     BackButton->OnClicked.AddDynamic(this, &USettingsWidget::HandleBackClicked);

	HideVideoConfirm();
	RefreshFromSettings();
}

void USettingsWidget::NativeDestruct()
{
	//확인 대기 중에 화면을 떠나면 타이머만 남아 죽은 위젯을 두드린다
	if (UWorld* world = GetWorld())
	{
		world->GetTimerManager().ClearTimer(videoConfirmTimerHandle);
	}

	Super::NativeDestruct();
}

void USettingsWidget::RefreshFromSettings()
{
	UPWGameUserSettings* settings = UPWGameUserSettings::Get();
	if (!settings) return;

	//해상도 — 모니터가 보고한 모드만 올린다
	if (ResolutionCombo)
	{
		resolutionOptions.Reset();
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(resolutionOptions);

		//현재 해상도가 목록에 없을 수 있다(창 모드에서 임의 크기), 그때도 고를 수 있게 끼워 넣는다
		const FIntPoint current = settings->GetScreenResolution();
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
		WindowModeCombo->SetSelectedIndex(static_cast<int32>(settings->GetFullscreenMode()));
	}

	if (VSyncCheck)
	{
		VSyncCheck->SetIsChecked(settings->IsVSyncEnabled());
	}

	if (FrameRateCombo)
	{
		FrameRateCombo->ClearOptions();
		for (const float limit : FrameRateOptions())
		{
			FrameRateCombo->AddOption(limit > 0.f ? FString::Printf(TEXT("%d"), FMath::RoundToInt(limit)) : TEXT("무제한"));
		}

		//저장된 상한이 후보에 없으면 무제한(마지막 항목)으로 표시한다
		const int32 index = FrameRateOptions().IndexOfByKey(settings->GetFrameRateLimit());
		FrameRateCombo->SetSelectedIndex(index != INDEX_NONE ? index : FrameRateOptions().Num() - 1);
	}

	if (EnemyTurnSpeedCombo)
	{
		const TArray<float>& speeds = UPWGameUserSettings::GetEnemyTurnSpeedOptions();
		EnemyTurnSpeedCombo->ClearOptions();
		for (const float speed : speeds)
		{
			EnemyTurnSpeedCombo->AddOption(FString::Printf(TEXT("%gx"), speed));
		}

		//ini를 손편집해 후보에 없는 값이 들어와도 선택이 비지 않도록 1x로 떨어뜨린다
		const int32 index = speeds.IndexOfByKey(settings->GetEnemyTurnSpeed());
		EnemyTurnSpeedCombo->SetSelectedIndex(index != INDEX_NONE ? index : 0);
	}

	if (EnemyTurnCameraCheck)
	{
		EnemyTurnCameraCheck->SetIsChecked(settings->IsEnemyTurnCameraFollowEnabled());
	}

	RefreshQualityCombos(settings);
}

void USettingsWidget::RefreshQualityCombos(const UPWGameUserSettings* settings)
{
	if (!settings) return;

	if (OverallQualityCombo)
	{
		FillQualityOptions(OverallQualityCombo, settings->GetOverallScalabilityLevel());
	}

	FillQualityOptions(ShadowQualityCombo, settings->GetShadowQuality());
	FillQualityOptions(TextureQualityCombo, settings->GetTextureQuality());
	FillQualityOptions(EffectQualityCombo, settings->GetVisualEffectQuality());
	FillQualityOptions(PostProcessQualityCombo, settings->GetPostProcessingQuality());
	FillQualityOptions(AntiAliasingQualityCombo, settings->GetAntiAliasingQuality());
	FillQualityOptions(ViewDistanceQualityCombo, settings->GetViewDistanceQuality());
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

void USettingsWidget::ApplyImmediate(UPWGameUserSettings* settings)
{
	settings->ApplyNonResolutionSettings();
	settings->SaveSettings();
}

void USettingsWidget::HandleVSyncChanged(bool bIsChecked)
{
	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetVSyncEnabled(bIsChecked);
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleFrameRateChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	//코드가 선택을 바꿀 때도 이 델리게이트가 Direct로 불린다, 초기화가 ini를 덮어쓰지 않도록 걸러낸다
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		const int32 index = FrameRateCombo->GetSelectedIndex();
		if (!FrameRateOptions().IsValidIndex(index)) return;

		settings->SetFrameRateLimit(FrameRateOptions()[index]);
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleOverallQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetOverallScalabilityLevel(OverallQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);

		//프리셋이 개별 축을 통째로 덮었으므로 표시를 다시 맞춘다
		RefreshQualityCombos(settings);
	}
}

void USettingsWidget::HandleShadowQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetShadowQuality(ShadowQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleTextureQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetTextureQuality(TextureQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleEffectQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetVisualEffectQuality(EffectQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandlePostProcessQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetPostProcessingQuality(PostProcessQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleAntiAliasingQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetAntiAliasingQuality(AntiAliasingQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleViewDistanceQualityChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetViewDistanceQuality(ViewDistanceQualityCombo->GetSelectedIndex());
		ApplyImmediate(settings);
	}
}

void USettingsWidget::HandleEnemyTurnSpeedChanged(FString selectedItem, ESelectInfo::Type selectInfo)
{
	if (selectInfo == ESelectInfo::Direct) return;

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		const TArray<float>& speeds = UPWGameUserSettings::GetEnemyTurnSpeedOptions();
		const int32 index = EnemyTurnSpeedCombo->GetSelectedIndex();
		if (!speeds.IsValidIndex(index)) return;

		settings->SetEnemyTurnSpeed(speeds[index]);

		//엔진 CVar가 아니라 적용할 것이 없다, 다음 적 턴이 시작될 때 읽힌다
		settings->SaveSettings();
	}
}

void USettingsWidget::HandleEnemyTurnCameraChanged(bool bIsChecked)
{
	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->SetEnemyTurnCameraFollowEnabled(bIsChecked);
		settings->SaveSettings();
	}
}

void USettingsWidget::HandleApplyClicked()
{
	UPWGameUserSettings* settings = UPWGameUserSettings::Get();
	if (!settings || !ResolutionCombo || !WindowModeCombo) return;

	const int32 resIndex = ResolutionCombo->GetSelectedIndex();
	if (!resolutionOptions.IsValidIndex(resIndex)) return;

	settings->SetScreenResolution(resolutionOptions[resIndex]);
	settings->SetFullscreenMode(EWindowMode::ConvertIntToWindowMode(WindowModeCombo->GetSelectedIndex()));
	settings->ApplyResolutionSettings(false);

	//아직 저장하지 않는다, 확인을 받아야 확정이다
	ShowVideoConfirm();
}

void USettingsWidget::ShowVideoConfirm()
{
	if (!ConfirmPanel) return;

	ConfirmPanel->SetVisibility(ESlateVisibility::Visible);
	videoConfirmRemaining = videoConfirmSeconds;

	if (ConfirmCountdownText)
	{
		ConfirmCountdownText->SetText(FText::AsNumber(videoConfirmRemaining));
	}

	if (UWorld* world = GetWorld())
	{
		world->GetTimerManager().SetTimer(videoConfirmTimerHandle, this,
			&USettingsWidget::TickVideoConfirm, 1.f, true);
	}
}

void USettingsWidget::HideVideoConfirm()
{
	if (ConfirmPanel)
	{
		ConfirmPanel->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (UWorld* world = GetWorld())
	{
		world->GetTimerManager().ClearTimer(videoConfirmTimerHandle);
	}
}

void USettingsWidget::TickVideoConfirm()
{
	--videoConfirmRemaining;

	if (videoConfirmRemaining <= 0)
	{
		RevertVideoMode();
		return;
	}

	if (ConfirmCountdownText)
	{
		ConfirmCountdownText->SetText(FText::AsNumber(videoConfirmRemaining));
	}
}

void USettingsWidget::RevertVideoMode()
{
	HideVideoConfirm();

	UPWGameUserSettings* settings = UPWGameUserSettings::Get();
	if (!settings) return;

	//RevertVideoMode는 필드만 되돌리므로 적용은 따로 불러야 한다
	settings->RevertVideoMode();
	settings->ApplyResolutionSettings(false);

	//되돌린 결과를 ini에도 반영한다, 적용 시점에 저장됐는지 여부와 무관하게 상태가 일치하도록
	settings->SaveSettings();

	RefreshFromSettings();
}

void USettingsWidget::HandleConfirmClicked()
{
	HideVideoConfirm();

	if (UPWGameUserSettings* settings = UPWGameUserSettings::Get())
	{
		settings->ConfirmVideoMode();
		settings->SaveSettings();
	}
}

void USettingsWidget::HandleDefaultsClicked()
{
	UPWGameUserSettings* settings = UPWGameUserSettings::Get();
	if (!settings) return;

	HideVideoConfirm();

	settings->SetToDefaults();

	//SetToDefaults는 해상도를 0으로 두므로 데스크톱 해상도로 메워야 한다
	settings->ValidateSettings();
	settings->ApplySettings(false);

	//기본값 해상도는 데스크톱 native라 되돌리기 확인이 필요 없다
	settings->ConfirmVideoMode();
	settings->SaveSettings();

	RefreshFromSettings();
}

void USettingsWidget::HandleBackClicked()
{
	//여는 쪽이 이 위젯을 제거할 수 있으므로 Execute 직후 아무것도 하지 않고 빠진다
	onClosed.ExecuteIfBound();
}