//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperRelocate.h"
#include "Characters/CharacterBase.h"

USniperRelocate::USniperRelocate()
{
	skillName = NSLOCTEXT("Skill", "SniperRelocate_Name", "위치 이탈");
	skillDescription = NSLOCTEXT("Skill", "SniperRelocate_Description", "적 처치 시 이동력 +6m");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterSlay;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USniperRelocate::Execute_AfterSlay(FVector slainLocation)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	o->GainMovingPoint(movingPointGain);
	PlayPassiveEffect();
}
