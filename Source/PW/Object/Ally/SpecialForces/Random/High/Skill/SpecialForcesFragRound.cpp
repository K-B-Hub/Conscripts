//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesFragRound.h"
#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesFragRoundDebuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USpecialForcesFragRound::USpecialForcesFragRound()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesFragRound_Name", "파쇄탄");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesFragRound_Description", "공격으로 피해를 준 대상에게 2턴간 방어 -8을 부여한다.");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesFragRoundDebuff::StaticClass());
}

void USpecialForcesFragRound::Execute_AfterDamage(ACharacterBase* target)
{
	if (!target || target->IsDead()) return;
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//광역에 휩쓸린 아군은 제외
	if (o->IsAlly() == target->IsAlly()) return;

	if (UBuffComponent* bc = target->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
