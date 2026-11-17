//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesCounterChance.h"
#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesCounterChanceBuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USpecialForcesCounterChance::USpecialForcesCounterChance()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesCounterChance_Name", "역공의 기회");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesCounterChance_Description", "공격을 회피하면 1턴간 명중 +15, 치명 +15");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterEvade;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesCounterChanceBuff::StaticClass());
}

void USpecialForcesCounterChance::Execute_AfterEvade(ACharacterBase* attacker)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;
	if (o->IsDead()) return;

	if (UBuffComponent* bc = o->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
