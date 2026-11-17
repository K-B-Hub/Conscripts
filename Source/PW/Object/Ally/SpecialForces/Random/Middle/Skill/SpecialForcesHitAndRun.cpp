//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesHitAndRun.h"
#include "Characters/CharacterBase.h"

USpecialForcesHitAndRun::USpecialForcesHitAndRun()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesHitAndRun_Name", "치고 빠지기");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesHitAndRun_Description", "치명타 적중 시 이동력 +3m");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterCritical;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USpecialForcesHitAndRun::Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	o->GainMovingPoint(movingPointGain);
	PlayPassiveEffect();
}
