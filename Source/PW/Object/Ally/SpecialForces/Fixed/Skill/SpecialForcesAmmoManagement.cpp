//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesAmmoManagement.h"
#include "Characters/CharacterBase.h"

USpecialForcesAmmoManagement::USpecialForcesAmmoManagement()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAmmoManagement_Name", "탄약 관리");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAmmoManagement_Description", "명중 +10, 턴 시작마다 장비를 1개 회복한다.");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TurnStart;

	//조건부 패시브는 스탯이 자동 적용되지 않아 Execute_Conditional에서 건다
	accuracy = 10.f;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesAmmoManagement::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	if (!bApplied)
	{
		bApplied = true;
		o->ApplyPassiveStatDelta(this, 1);
	}

	//최대치 초과분은 GainBattleResource가 클램프
	const int32 before = o->GetBattleResource();
	o->GainBattleResource(resourceGain);
	if (o->GetBattleResource() != before) PlayPassiveEffect();
}
