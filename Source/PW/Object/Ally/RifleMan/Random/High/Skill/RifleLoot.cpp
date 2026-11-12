//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleLoot.h"
#include "Characters/CharacterBase.h"

URifleLoot::URifleLoot()
{
	skillName = NSLOCTEXT("Skill", "RifleLoot_Name", "전리품 획득");
	skillDescription = NSLOCTEXT("Skill", "RifleLoot_Description", "적 처치 시 탄창을 3개 회복한다.");
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
	const int32 before = o->GetBattleResource();
	o->GainBattleResource(resourceGain);
	//실제로 회복됐을 때만 연출
	if (o->GetBattleResource() != before) PlayPassiveEffect();
}
