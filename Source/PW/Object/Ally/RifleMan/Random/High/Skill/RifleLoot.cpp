//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleLoot.h"
#include "Characters/CharacterBase.h"

URifleLoot::URifleLoot()
{
	skillName = NSLOCTEXT("Skill", "RifleLoot_Name", "전리품 획득");
	skillDescription = NSLOCTEXT("Skill", "RifleLoot_Description", "적을 처치하면 탄창을 3개 회복한다");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterSlay;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;
}

void URifleLoot::Execute_AfterSlay(FVector slainLocation)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//최대치 초과분은 GainBattleResource가 클램프
	o->GainBattleResource(resourceGain);
}
