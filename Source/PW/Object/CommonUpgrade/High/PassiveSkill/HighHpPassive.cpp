//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighHpPassive.h"

UHighHpPassive::UHighHpPassive()
{
	skillName = NSLOCTEXT("Skill", "HighHp_Name", "상급 체력 증가");
	skillDescription = NSLOCTEXT("Skill", "HighHp_Description", "최대 체력이 5 증가한다");

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//스탯 성장형이라 중복 획득으로 누적 가능
	bAllowDuplicate = true;

	hp = 5;
}
