//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/Middle/Skill/SpecialForcesAfterimage.h"
#include "Object/Ally/SpecialForces/Random/Middle/Buff/SpecialForcesAfterimageBuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USpecialForcesAfterimage::USpecialForcesAfterimage()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesAfterimage_Name", "잔영");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesAfterimage_Description", "공격을 회피하면 1턴간 이동력 +2m(최대 3중첩)");
	skillGrade = EUpgradeGrade::Mid;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterEvade;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesAfterimageBuff::StaticClass());
}

void USpecialForcesAfterimage::Execute_AfterEvade(ACharacterBase* attacker)
{
	ACharacterBase* o = owner.Get();
	if (!o) return;
	if (o->IsDead()) return;

	//중첩 상한은 버프의 maxStack이 막는다
	if (UBuffComponent* bc = o->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
