//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesSwiftResupply.h"
#include "Object/Ally/SpecialForces/Basic/SpecialForcesResupply.h"

USpecialForcesSwiftResupply::USpecialForcesSwiftResupply()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSwiftResupply_Name", "신속한 정비");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSwiftResupply_Description", "장비 정비의 행동력 소모 -1");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	//파생 관계로 판정하므로 BP_SpecialForcesResupply도 같은 할인을 받는다
	discountSkillClass = USpecialForcesResupply::StaticClass();
	actionPointDiscount = 1;
}
