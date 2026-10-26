//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Top/Skill/RifleThrowMaster.h"

URifleThrowMaster::URifleThrowMaster()
{
	skillName = NSLOCTEXT("Skill", "RifleThrowMaster_Name", "투척술의 대가");
	skillDescription = NSLOCTEXT("Skill", "RifleThrowMaster_Description", "투척 공격에 치명타가 적용될 수 있다");
	skillGrade = EUpgradeGrade::Top;

	//스탯 변화 없이 봉쇄만 푼다, 조건 검사도 필요 없다
	passiveType = EPassiveType::Stat;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::None;

	//효과가 on/off라 중복 획득해도 의미 없음
	bAllowDuplicate = false;

	bAllowThrowCritical = true;
}
