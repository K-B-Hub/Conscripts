// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/BattleTurnWidget.h"

#include "Widget/MoveWidget.h"
#include "Widget/SkillWidget.h"
#include "Widget/CircularGaugeWidget.h"
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
			SkillWidget->InitSkills(SkillComp);
		}
	}

	if (HpGauge)
	{
		HpGauge->InitGauge(TurnUnit->GetMaxHp(), TurnUnit->GetHp());
	}
	if (StressGauge)
	{
		StressGauge->InitGauge(TurnUnit->GetMaxStress(), TurnUnit->GetStress());
	}
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

void UBattleTurnWidget::RefreshGauges(int32 InHp, int32 InStress)
{
	if (HpGauge)
	{
		HpGauge->SetValue(InHp);
	}
	if (StressGauge)
	{
		StressGauge->SetValue(InStress);
	}
}
