//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperExposeWeakness.h"
#include "Object/Ally/Sniper/Random/High/Buff/SniperExposeWeaknessDebuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USniperExposeWeakness::USniperExposeWeakness()
{
	skillName = NSLOCTEXT("Skill", "SniperExposeWeakness_Name", "약점 포착");
	skillDescription = NSLOCTEXT("Skill", "SniperExposeWeakness_Description", "공격으로 피해를 준 대상에게 2턴간 받는 피해 +10%를 부여한다.");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperExposeWeaknessDebuff::StaticClass());
}

void USniperExposeWeakness::Execute_AfterDamage(ACharacterBase* target)
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
