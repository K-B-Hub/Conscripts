//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillBannerWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Characters/CharacterBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "GameMode/BattleGameMode.h"
#include "Settings/PWGameUserSettings.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

void USkillBannerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Collapsed);

	cachedGameMode = Cast<ABattleGameMode>(GetWorld()->GetAuthGameMode());
	if (cachedGameMode)
	{
		cachedGameMode->OnSkillAnnounced.AddUObject(this, &USkillBannerWidget::HandleSkillAnnounced);
	}
}

void USkillBannerWidget::NativeDestruct()
{
	if (cachedGameMode)
	{
		cachedGameMode->OnSkillAnnounced.RemoveAll(this);
		cachedGameMode = nullptr;
	}
	if (UWorld* world = GetWorld()) world->GetTimerManager().ClearTimer(hideTimer);

	Super::NativeDestruct();
}

void USkillBannerWidget::HandleSkillAnnounced(ACharacterBase* caster, const UActiveSkillBase* skill)
{
	if (!caster || !skill || displayDuration <= 0.f) return;

	//설정 화면 변경이 다음 시전부터 바로 반영되도록 매번 읽는다
	const UPWGameUserSettings* settings = UPWGameUserSettings::Get();
	const ESkillCameraMode mode = settings ? settings->GetSkillBannerMode() : ESkillCameraMode::EnemyOnly;
	const bool bAlly = caster->IsAlly();
	const bool bShow = mode == ESkillCameraMode::All
		|| (mode == ESkillCameraMode::AllyOnly && bAlly)
		|| (mode == ESkillCameraMode::EnemyOnly && !bAlly);
	if (!bShow) return;

	//적은 고유 이름을 지정하지 않으므로 직업명
	CasterNameText->SetText(bAlly ? caster->GetCharacterNameText() : caster->GetJobNameText());
	SkillNameText->SetText(skill->skillName);
	if (SkillIcon)
	{
		SkillIcon->SetBrushFromTexture(skill->skillIcon);
		SkillIcon->SetVisibility(skill->skillIcon ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

	//연속 시전이면 타이머만 다시 건다
	SetVisibility(ESlateVisibility::HitTestInvisible);
	if (ShowAnim) PlayAnimation(ShowAnim);
	if (announceSound) UGameplayStatics::PlaySound2D(this, announceSound);

	GetWorld()->GetTimerManager().SetTimer(hideTimer, this, &USkillBannerWidget::HideBanner, displayDuration, false);
}

void USkillBannerWidget::HideBanner()
{
	SetVisibility(ESlateVisibility::Collapsed);
}
