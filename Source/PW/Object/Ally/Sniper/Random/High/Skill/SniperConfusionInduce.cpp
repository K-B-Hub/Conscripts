//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperConfusionInduce.h"
#include "Object/Ally/Sniper/Random/High/Buff/SniperConfusionDebuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USniperConfusionInduce::USniperConfusionInduce()
{
	skillName = NSLOCTEXT("Skill", "SniperConfusionInduce_Name", "혼란 유발");
	skillDescription = NSLOCTEXT("Skill", "SniperConfusionInduce_Description", "공격으로 피해를 준 대상에게 20% 확률로 1턴간 혼란(이동 불가, 명중 -50, 회피 -50)을 부여한다.");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USniperConfusionDebuff::StaticClass());
}

void USniperConfusionInduce::Execute_AfterDamage(ACharacterBase* target)
{
	if (!target || target->IsDead()) return;
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//광역에 휩쓸린 아군은 제외
	if (o->IsAlly() == target->IsAlly()) return;

	if (FMath::FRandRange(0.f, 100.f) >= chancePercent) return;

	if (UBuffComponent* bc = target->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
