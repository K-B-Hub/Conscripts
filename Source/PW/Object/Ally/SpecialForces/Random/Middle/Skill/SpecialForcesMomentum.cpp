//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesMomentum.h"
#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesMomentumBuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USpecialForcesMomentum::USpecialForcesMomentum()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesMomentum_Name", "고조");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesMomentum_Description", "적 처치 시 2턴간 회피 +25");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterSlay;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesMomentumBuff::StaticClass());
}

void USpecialForcesMomentum::Execute_AfterSlay(FVector slainLocation)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	if (UBuffComponent* bc = o->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
