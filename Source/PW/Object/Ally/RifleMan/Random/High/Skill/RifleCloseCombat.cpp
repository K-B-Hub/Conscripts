//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleCloseCombat.h"

URifleCloseCombat::URifleCloseCombat()
{
	skillName = NSLOCTEXT("Skill", "RifleCloseCombat_Name", "근접 전투술");
	skillDescription = NSLOCTEXT("Skill", "RifleCloseCombat_Description", "근접 공격의 행동력 소모가 1 감소한다");
	skillGrade = EUpgradeGrade::High;

	//스탯 변화 없이 비용만 깎는다, 조건 검사도 필요 없다
	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//할인이 중첩되면 근접 공격이 공짜에 가까워진다
	bAllowDuplicate = false;

	//근접 스킬은 클래스가 여럿이라 유형으로 지정한다
	discountSkillTypes.Add(ESkillType::Melee);
	actionPointDiscount = 1;
}
