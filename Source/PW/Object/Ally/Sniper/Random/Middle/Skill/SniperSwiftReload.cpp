//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Middle/Skill/SniperSwiftReload.h"
#include "Object/Ally/Sniper/Basic/SniperReload.h"

USniperSwiftReload::USniperSwiftReload()
{
	skillName = NSLOCTEXT("Skill", "SniperSwiftReload_Name", "신속한 장전");
	skillDescription = NSLOCTEXT("Skill", "SniperSwiftReload_Description", "재장전의 행동력 소모 -1");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;

	//파생 관계로 판정하므로 BP_SniperReload도 같은 할인을 받는다
	discountSkillClass = USniperReload::StaticClass();
	actionPointDiscount = 1;
}
