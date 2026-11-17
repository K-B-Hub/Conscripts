//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperNeurotoxin.h"
#include "Object/Ally/Sniper/Random/High/Buff/SniperNeurotoxinDebuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USniperNeurotoxin::USniperNeurotoxin()
{
	skillName = NSLOCTEXT("Skill", "SniperNeurotoxin_Name", "신경독탄");
	skillDescription = NSLOCTEXT("Skill", "SniperNeurotoxin_Description", "공격으로 피해를 준 대상에게 2턴간 이동력 -2m를 부여한다.");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperNeurotoxinDebuff::StaticClass());
}

void USniperNeurotoxin::Execute_AfterDamage(ACharacterBase* target)
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
