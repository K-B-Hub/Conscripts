//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/Top/Skill/SniperReaper.h"
#include "Characters/CharacterBase.h"

USniperReaper::USniperReaper()
{
	skillName = NSLOCTEXT("Skill", "SniperReaper_Name", "사신");
	skillDescription = NSLOCTEXT("Skill", "SniperReaper_Description", "적 처치 시 행동력 1 회복");
	skillGrade = EUpgradeGrade::Top;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterSlay;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;
}

void USniperReaper::Execute_AfterSlay(FVector slainLocation)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//음수 차감이 곧 회복, 최대치 초과분은 ReduceActionPoint가 클램프
	const int32 before = o->GetCurrentActionPoint();
	o->ReduceActionPoint(-actionPointGain);
	if (o->GetCurrentActionPoint() != before) PlayPassiveEffect();
}
