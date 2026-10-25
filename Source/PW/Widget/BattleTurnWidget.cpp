// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/BattleTurnWidget.h"

#include "Widget/MoveWidget.h"
#include "Widget/SkillWidget.h"
#include "Widget/CircularGaugeWidget.h"
#include "Widget/MovingPointWidget.h"
#include "Widget/StatusEffectIcon.h"
#include "Characters/AllyCharacterBase.h"
#include "ActorComponent/SkillComponent.h"

void UBattleTurnWidget::InitTurnHUD(AAllyCharacterBase* TurnUnit)
{
	if (!IsValid(TurnUnit)) return;

	if (MoveWidget)
	{
		MoveWidget->RefreshMoveButton(TurnUnit->GetCurrentMovingPoint());
	}

	if (SkillWidget)
	{
		if (USkillComponent* SkillComp = TurnUnit->GetSkillComponent())
		{
			SkillComp->CalcSkillStats();
			SkillWidget->InitSkills(TurnUnit);
		}
	}

	if (MovingPointWidget)
	{
		MovingPointWidget->InitMovingPoint(TurnUnit);
	}

	//게이지 넷은 초기화와 갱신이 같은 일이다
	RefreshGauges(TurnUnit);
	RefreshStatusEffects(TurnUnit);
}

void UBattleTurnWidget::RefreshMoveButton(float currentMovingPoint)
{
	if (MoveWidget)
	{
		MoveWidget->RefreshMoveButton(currentMovingPoint);
	}
}

void UBattleTurnWidget::RefreshSkillButtons()
{
	if (SkillWidget)
	{
		SkillWidget->RefreshButtons();
	}
}

void UBattleTurnWidget::RefreshGauges(ACharacterBase* unit)
{
	if (!IsValid(unit)) return;

	if (HpGauge)
	{
		HpGauge->InitGauge(unit->GetMaxHp(), unit->GetHp());
	}
	if (StressGauge)
	{
		StressGauge->InitGauge(unit->GetMaxStress(), unit->GetStress());
	}
	if (ActionPointGauge)
	{
		ActionPointGauge->InitGauge(unit->GetActionPoint(), unit->GetCurrentActionPoint());
	}
	if (BattleResourceGauge)
	{
		BattleResourceGauge->InitGauge(unit->GetMaxBattleResource(), unit->GetBattleResource());
	}
}

void UBattleTurnWidget::RefreshStatusEffects(ACharacterBase* unit)
{
	UStatusEffectIcon::FillContainer(StatusEffectBox, statusIconClass, unit, this);
}
