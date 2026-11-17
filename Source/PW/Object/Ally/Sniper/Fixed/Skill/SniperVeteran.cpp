//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Fixed/Skill/SniperVeteran.h"
#include "Characters/CharacterBase.h"

USniperVeteran::USniperVeteran()
{
	skillName = NSLOCTEXT("Skill", "SniperVeteran_Name", "노련한 저격수");
	skillDescription = NSLOCTEXT("Skill", "SniperVeteran_Description", "턴 시작마다 탄환을 1개 회복한다.");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TurnStart;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USniperVeteran::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//최대치 초과분은 GainBattleResource가 클램프
	const int32 before = o->GetBattleResource();
	o->GainBattleResource(resourceGain);
	//실제로 회복됐을 때만 연출
	if (o->GetBattleResource() != before) PlayPassiveEffect();
}
