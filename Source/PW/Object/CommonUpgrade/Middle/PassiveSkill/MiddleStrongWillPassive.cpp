//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Middle/PassiveSkill/MiddleStrongWillPassive.h"
#include "Characters/CharacterBase.h"

UMiddleStrongWillPassive::UMiddleStrongWillPassive()
{
	skillName = NSLOCTEXT("Skill", "MiddleStrongWill_Name", "굳센 의지");
	skillDescription = NSLOCTEXT("Skill", "MiddleStrongWill_Description", "체력이 40% 이하일 때 회피와 명중이 25 증가한다");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::Damaged;

	//조건 충족 여부를 인스턴스가 래치하므로 중복 획득 불가
	bAllowDuplicate = false;

	evasion = 25.0f;
	accuracy = 25.0f;
}

void UMiddleStrongWillPassive::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//Damaged는 회복에도 발동하므로 매번 조건을 다시 판정
	const bool bShouldApply = o->GetHp() <= o->GetMaxHp() * hpThreshold;
	if (bShouldApply == bApplied) return;

	bApplied = bShouldApply;
	o->ApplyPassiveStatDelta(this, bApplied ? 1 : -1);
}