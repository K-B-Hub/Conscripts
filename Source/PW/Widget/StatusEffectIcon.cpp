//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/StatusEffectIcon.h"
#include "Widget/SkillTooltipWidget.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "ActorComponent/AilmentComponent.h"
#include "Object/Buff/BuffBase.h"
#include "Object/Ailment/AilmentBase.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Components/Image.h"
#include "Components/PanelWidget.h"
#include "Engine/Texture2D.h"

namespace
{
	//이름 미지정 BP면 클래스명으로 대신한다, 스킬 목록과 같은 폴백
	FText NameOrClass(const FText& name, const UObject* object)
	{
		return name.IsEmpty() ? FText::FromString(object->GetClass()->GetName()) : name;
	}

	TArray<FStatusEffectView> CollectViews(const ACharacterBase* target)
	{
		TArray<FStatusEffectView> views;

		if (const UBuffComponent* buffComp = target->GetBuffComponent())
		{
			for (const UBuffBase* buff : buffComp->GetActiveBuffs())
			{
				if (!buff) continue;
				views.Add({ buff->buffIcon.Get(), NameOrClass(buff->buffName, buff), buff->buffDescription, buff->remainingTurn });
			}
		}

		//조건부로 스탯을 가감 중인 패시브, 습득 즉시 영구 적용되는 Stat 타입은 걸린 효과가 아니라 제외
		for (const TPair<TWeakObjectPtr<const UPassiveSkillBase>, int32>& pair : target->GetPassiveDeltaStacks())
		{
			const UPassiveSkillBase* passive = pair.Key.Get();
			if (!passive) continue;

			//중첩형은 스킬 목록처럼 개수를 붙인다
			const FText name = NameOrClass(passive->skillName, passive);
			views.Add({ passive->skillIcon.Get(),
				pair.Value >= 2 ? FText::FromString(FString::Printf(TEXT("%s x %d"), *name.ToString(), pair.Value)) : name,
				passive->skillDescription });
		}

		if (const UAilmentComponent* ailmentComp = target->GetAilmentComponent())
		{
			for (const UAilmentBase* ailment : ailmentComp->GetActiveAilments())
			{
				if (!ailment) continue;
				views.Add({ ailment->ailmentIcon.Get(), NameOrClass(ailment->ailmentName, ailment), ailment->ailmentDescription, ailment->remainingTurn });
			}
		}

		return views;
	}
}

void UStatusEffectIcon::FillContainer(UPanelWidget* container, TSubclassOf<UStatusEffectIcon> iconClass,
	const ACharacterBase* target, UUserWidget* owner)
{
	if (!container) return;

	container->ClearChildren();
	if (!iconClass || !IsValid(target) || !owner) return;

	for (const FStatusEffectView& view : CollectViews(target))
	{
		UStatusEffectIcon* entry = CreateWidget<UStatusEffectIcon>(owner, iconClass);
		if (!entry) continue;

		entry->InitEntry(view);
		container->AddChild(entry);
	}
}

void UStatusEffectIcon::InitEntry(const FStatusEffectView& view)
{
	if (EffectIcon)
	{
		//아이콘 미지정이면 그림만 감춘다, 칸은 남아 툴팁으로 정체를 확인할 수 있다
		if (view.icon)
		{
			EffectIcon->SetBrushFromTexture(view.icon);
			EffectIcon->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			EffectIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (!tooltipClass) return;

	if (USkillTooltipWidget* tooltip = CreateWidget<USkillTooltipWidget>(GetOwningPlayer(), tooltipClass))
	{
		tooltip->InitStatusTooltip(view);
		SetToolTip(tooltip);
	}

	//루트·SizeBox·아이콘이 모두 히트 테스트에서 빠지면 호버가 닿지 않아 툴팁이 안 뜬다
	SetVisibility(ESlateVisibility::Visible);
}
