//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Fixed/Skill/SpecialForcesSmokeGrenade.h"
#include "PW.h"

USpecialForcesSmokeGrenade::USpecialForcesSmokeGrenade()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesSmokeGrenade_Name", "연막탄 투척");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesSmokeGrenade_Description", "6m 내의 지점에 던져 반경 1m를 1라운드간 연막 지형으로 만든다. 연막 안에 있는 캐릭터는 회피 +30");
	//부모가 중급 랜덤 강화라 고정 강화 등급으로 되돌린다
	skillGrade = EUpgradeGrade::Low;

	pickRange = 6.f * CmPerMeter;

	battleResourceCost = 1;
	actionPointCost = 2;
}
