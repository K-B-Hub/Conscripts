//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CharacterDetailWidget.h"
#include "Widget/StatTextHelpers.h"
#include "Widget/SkillListEntry.h"
#include "Widget/StatusEffectIcon.h"
#include "Widget/SkillTooltipWidget.h"
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

namespace
{
	//목록 항목 하나, 같은 클래스 스킬을 묶은 결과
	struct FEntryGroup
	{
		USkillBase* skill = nullptr;
		int32 count = 0;
	};

	//화살표만 현재 방향으로 바꾼다, 위치용 띄어쓰기는 WBP 텍스트 그대로, ▼는 최상급이 위 ▲는 일반이 위
	void ApplySortText(UTextBlock* text, bool bDescending)
	{
		if (!text) return;

		FString label = text->GetText().ToString();
		label.ReplaceInline(bDescending ? TEXT("▲") : TEXT("▼"), bDescending ? TEXT("▼") : TEXT("▲"));
		text->SetText(FText::FromString(label));
	}
}

void UCharacterDetailWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UCharacterDetailWidget::HandleCloseClicked);
	if (ActiveSortButton) ActiveSortButton->OnClicked.AddDynamic(this, &UCharacterDetailWidget::HandleActiveSortClicked);
	if (PassiveSortButton) PassiveSortButton->OnClicked.AddDynamic(this, &UCharacterDetailWidget::HandlePassiveSortClicked);

	//스탯 설명은 대상과 무관해 생성 시 한 번만 건다, 계산식 변경 시 함께 수정
	AttachStatTooltip(HpText, NSLOCTEXT("StatTooltip", "Hp",
		"0이 되면 사망합니다.\n비치명피해로는 1 아래로 내려가지 않습니다."));
	AttachStatTooltip(BattleResourceText, NSLOCTEXT("StatTooltip", "BattleResource",
		"일부 스킬을 사용할 때 소모하는 고유 자원입니다."));
	AttachStatTooltip(AtkText, NSLOCTEXT("StatTooltip", "Atk",
		"스킬 피해의 기반이 됩니다.\n스킬 피해 = 공격력 × 스킬 계수 + 스킬 고정 피해"));
	AttachStatTooltip(SpeedText, NSLOCTEXT("StatTooltip", "Speed",
		"턴 순서와 회피에 영향을 줍니다."));
	AttachStatTooltip(SkillText, NSLOCTEXT("StatTooltip", "Skill",
		"명중과 치명에 영향을 줍니다."));
	AttachStatTooltip(DefText, NSLOCTEXT("StatTooltip", "Def",
		"받는 피해에서 고정 수치를 뺍니다."));
	AttachStatTooltip(MentalityText, NSLOCTEXT("StatTooltip", "Mentality",
		"받는 스트레스를 줄입니다."));
	AttachStatTooltip(ActionPointText, NSLOCTEXT("StatTooltip", "ActionPoint",
		"한 턴에 쓸 수 있는 행동력의 최대치입니다.\n스킬마다 정해진 만큼 소모하고 턴 시작 시 최대치로 회복합니다."));
	AttachStatTooltip(MovingPointText, NSLOCTEXT("StatTooltip", "MovingPoint",
		"한 턴에 이동할 수 있는 거리(m)입니다.\n지형과 포복 자세에 따라 1m당 소모량이 늘어나며 턴 시작 시 회복합니다."));
	AttachStatTooltip(SightText, NSLOCTEXT("StatTooltip", "Sight",
		"시야 패널티가 시작되는 거리(m)입니다.\n시야를 넘는 1m마다 명중이 10씩 떨어집니다."));
	AttachStatTooltip(AccuracyText, NSLOCTEXT("StatTooltip", "Accuracy",
		"공격과 상태이상이 맞을 확률을 정합니다.\n명중률 = 명중 - 대상 회피 - 시야 패널티 (0 ~ 100%)"));
	AttachStatTooltip(EvasionText, NSLOCTEXT("StatTooltip", "Evasion",
		"상대의 명중률을 그대로 깎습니다.\n기본값 = 속도 × 1.2"));
	AttachStatTooltip(CriticalText, NSLOCTEXT("StatTooltip", "Critical",
		"공격이 치명타로 들어갈 확률(%)입니다.\n기본값 = 기술 × 0.5, 광역 공격은 치명타가 터지지 않습니다."));
	AttachStatTooltip(PenetrationText, NSLOCTEXT("StatTooltip", "Penetration",
		"대상의 방어를 비율로 무시합니다."));
	AttachStatTooltip(DamageAmplificationText, NSLOCTEXT("StatTooltip", "DamageAmplification",
		"주는 피해와 회복량을 비율로 늘립니다."));
	AttachStatTooltip(DamageReductionText, NSLOCTEXT("StatTooltip", "DamageReduction",
		"받는 피해를 비율로 줄입니다."));
}

void UCharacterDetailWidget::AttachStatTooltip(UTextBlock* valueText, const FText& description)
{
	if (!valueText || !statTooltipClass) return;

	UTextBlock* label = Cast<UTextBlock>(GetWidgetFromName(*(TEXT("Label_") + valueText->GetName())));

	FStatusEffectView view;
	view.name = label ? label->GetText() : FText::GetEmpty();
	view.description = description;

	for (UTextBlock* cell : { label, valueText })
	{
		if (!cell) continue;

		//한 툴팁 위젯을 두 칸이 공유하면 Slate 부모가 겹치므로 칸마다 만든다
		USkillTooltipWidget* tooltip = CreateWidget<USkillTooltipWidget>(GetOwningPlayer(), statTooltipClass);
		if (!tooltip) continue;

		tooltip->InitStatusTooltip(view);
		cell->SetToolTip(tooltip);
		//TextBlock 기본값 SelfHitTestInvisible은 호버가 닿지 않는다
		cell->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCharacterDetailWidget::InitDetail(ACharacterBase* target)
{
	if (!target) return;

	//직업 이름 미지정 BP면 클래스명으로 대신한다, 편성·출격 화면과 같은 폴백
	if (JobNameText)      JobNameText->SetText(target->GetJobNameText());
	if (CharacterNameText) CharacterNameText->SetText(target->GetCharacterNameText());

	//고유 초상화가 없으면 직업 아이콘이 나온다
	if (JobImage)
	{
		if (UTexture2D* art = target->GetPortrait())
		{
			JobImage->SetBrushFromTexture(art);
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

	activeSkills.Reset();
	if (const USkillComponent* skillComp = target->GetSkillComponent())
	{
		for (UActiveSkillBase* skill : skillComp->GetActiveSkills())
		{
			activeSkills.Add(skill);
		}
	}
	FillSkillList(ActiveSkillList, activeSkills, bActiveDescending);
	ApplySortText(ActiveSortText, bActiveDescending);

	//GetActivePassives()는 "활성 중인 전체 패시브"다, 타입별 배열은 디스패치용 인덱스일 뿐이다
	passiveSkills.Reset();
	if (const UPassiveSkillComponent* passiveComp = target->GetPassiveSkillComponent())
	{
		for (const TObjectPtr<UPassiveSkillBase>& passive : passiveComp->GetActivePassives())
		{
			passiveSkills.Add(passive);
		}
	}
	FillSkillList(PassiveSkillList, passiveSkills, bPassiveDescending);
	ApplySortText(PassiveSortText, bPassiveDescending);

	UStatusEffectIcon::FillContainer(StatusEffectList, statusIconClass, target, this);
}

void UCharacterDetailWidget::FillSkillList(UPanelWidget* list, const TArray<TObjectPtr<USkillBase>>& skills, bool bDescending)
{
	if (!list || !skillEntryClass) return;

	list->ClearChildren();

	//같은 클래스는 한 항목으로 묶어 개수를 센다, 중복은 bAllowDuplicate 패시브에서만 생긴다
	TArray<FEntryGroup> groups;
	for (const TObjectPtr<USkillBase>& skill : skills)
	{
		if (!skill) continue;

		FEntryGroup* found = groups.FindByPredicate([&skill](const FEntryGroup& group)
			{ return group.skill->GetClass() == skill->GetClass(); });

		if (found) ++found->count;
		else groups.Add({ skill.Get(), 1 });
	}

	//등급만 비교하므로 StableSort로 같은 등급 안에서는 습득 순서가 유지된다
	groups.StableSort([bDescending](const FEntryGroup& a, const FEntryGroup& b)
		{
			return bDescending
				? a.skill->skillGrade > b.skill->skillGrade
				: a.skill->skillGrade < b.skill->skillGrade;
		});

	for (const FEntryGroup& group : groups)
	{
		USkillListEntry* entry = CreateWidget<USkillListEntry>(this, skillEntryClass);
		if (!entry) continue;

		//AddChild가 NativeConstruct를 부르므로 InitEntry가 먼저 와야 툴팁이 채워진다
		entry->InitEntry(group.skill, group.count);
		list->AddChild(entry);
	}
}

void UCharacterDetailWidget::HandleCloseClicked()
{
	OnClosed.ExecuteIfBound();
}

void UCharacterDetailWidget::HandleActiveSortClicked()
{
	bActiveDescending = !bActiveDescending;
	FillSkillList(ActiveSkillList, activeSkills, bActiveDescending);
	ApplySortText(ActiveSortText, bActiveDescending);
}

void UCharacterDetailWidget::HandlePassiveSortClicked()
{
	bPassiveDescending = !bPassiveDescending;
	FillSkillList(PassiveSkillList, passiveSkills, bPassiveDescending);
	ApplySortText(PassiveSortText, bPassiveDescending);
}