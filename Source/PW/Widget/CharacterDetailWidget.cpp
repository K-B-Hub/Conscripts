//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CharacterDetailWidget.h"
#include "Widget/StatTextHelpers.h"
#include "Widget/SkillListEntry.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UCharacterDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UCharacterDetailWidget::HandleCloseClicked);
}

void UCharacterDetailWidget::InitDetail(ACharacterBase* target)
{
	if (!target) return;

	if (JobNameText)
	{
		//직업 이름 미지정 BP면 클래스명으로 대신한다, 편성·출격 화면과 같은 폴백
		JobNameText->SetText(target->jobName.IsEmpty()
			? FText::FromString(target->GetClass()->GetName())
			: target->jobName);
	}

	if (JobImage)
	{
		if (target->jobIcon)
		{
			JobImage->SetBrushFromTexture(target->jobIcon);
			JobImage->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			JobImage->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (LevelText) LevelText->SetText(FText::FromString(FString::Printf(TEXT("LV. %d"), target->GetLevel())));

	PWStatText::SetPair(HpText, target->GetHp(), target->GetMaxHp());
	PWStatText::SetPair(BattleResourceText, target->GetBattleResource(), target->GetMaxBattleResource());

	PWStatText::SetInt(AtkText, target->GetAtk());
	PWStatText::SetInt(SpeedText, target->GetSpeed());
	PWStatText::SetInt(SkillText, target->GetSkill());
	PWStatText::SetInt(DefText, target->GetDef());
	PWStatText::SetInt(MentalityText, target->GetMentality());

	//행동력은 턴마다 소모되는 현재치가 아니라 이 캐릭터의 최대치를 보여준다
	PWStatText::SetInt(ActionPointText, target->GetActionPoint());
	PWStatText::SetFloat(MovingPointText, target->GetMovingPoint());
	PWStatText::SetFloat(SightText, target->GetSight());

	//accuracy·evasion은 뺄셈식 acc-eva에 들어가는 원시 수치라 단독으로는 %가 아니다
	//명중 수치의 100 초과분은 시야 초과 감쇠를 버티는 여유분으로 쓰인다
	PWStatText::SetFloat(AccuracyText, target->GetAccuracy());
	PWStatText::SetFloat(EvasionText, target->GetEvasion());
	PWStatText::SetFloat(CriticalText, target->GetCritical());
	PWStatText::SetInt(PenetrationText, target->GetPenetration());
	PWStatText::SetInt(DamageAmplificationText, target->GetDamageAmplification());
	PWStatText::SetInt(DamageReductionText, target->GetDamageReduction());

	if (const USkillComponent* skillComp = target->GetSkillComponent())
	{
		TArray<USkillBase*> actives;
		for (UActiveSkillBase* skill : skillComp->GetActiveSkills())
		{
			actives.Add(skill);
		}
		FillSkillList(ActiveSkillList, actives);
	}

	//GetActivePassives()는 "활성 중인 전체 패시브"다, 타입별 배열은 디스패치용 인덱스일 뿐이다
	if (const UPassiveSkillComponent* passiveComp = target->GetPassiveSkillComponent())
	{
		TArray<USkillBase*> passives;
		for (const TObjectPtr<UPassiveSkillBase>& passive : passiveComp->GetActivePassives())
		{
			passives.Add(passive);
		}
		FillSkillList(PassiveSkillList, passives);
	}
}

void UCharacterDetailWidget::FillSkillList(UPanelWidget* list, const TArray<USkillBase*>& skills)
{
	if (!list || !skillEntryClass) return;

	list->ClearChildren();

	for (USkillBase* skill : skills)
	{
		if (!skill) continue;

		USkillListEntry* entry = CreateWidget<USkillListEntry>(this, skillEntryClass);
		if (!entry) continue;

		//AddChild가 NativeConstruct를 부르므로 InitEntry가 먼저 와야 툴팁이 채워진다
		entry->InitEntry(skill);
		list->AddChild(entry);
	}
}

void UCharacterDetailWidget::HandleCloseClicked()
{
	OnClosed.ExecuteIfBound();
}