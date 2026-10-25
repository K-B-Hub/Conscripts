//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/RifleSwiftReload.h"
#include "Object/Ally/RifleMan/Basic/RifleReload.h"

URifleSwiftReload::URifleSwiftReload()
{
	skillName = NSLOCTEXT("Skill", "RifleSwiftReload_Name", "신속한 재장전");
	skillDescription = NSLOCTEXT("Skill", "RifleSwiftReload_Description", "재장전의 행동력 소모가 1 감소한다");
	skillGrade = EUpgradeGrade::Mid;

	//스탯 변화 없이 비용만 깎는다, 조건 검사도 필요 없다
	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//할인이 중첩되면 재장전이 공짜에 가까워진다
	bAllowDuplicate = false;

	//파생 관계로 판정하므로 BP_RifleReload도 같은 할인을 받는다
	discountSkillClass = URifleReload::StaticClass();
	actionPointDiscount = 1;
}