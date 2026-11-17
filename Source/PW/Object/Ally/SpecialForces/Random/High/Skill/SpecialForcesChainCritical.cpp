//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesChainCritical.h"
#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesChainCriticalBuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USpecialForcesChainCritical::USpecialForcesChainCritical()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesChainCritical_Name", "연쇄 치명");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesChainCritical_Description", "치명타 적중 시 2턴간 치명 +10(최대 3중첩)");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterCritical;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesChainCriticalBuff::StaticClass());
}

void USpecialForcesChainCritical::Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//중첩 상한은 버프의 maxStack이 막는다
	if (UBuffComponent* bc = o->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
