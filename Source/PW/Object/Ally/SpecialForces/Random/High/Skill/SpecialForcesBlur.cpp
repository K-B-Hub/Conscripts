//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Random/High/Skill/SpecialForcesBlur.h"
#include "Object/Ally/SpecialForces/Random/High/Buff/SpecialForcesBlurBuff.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

USpecialForcesBlur::USpecialForcesBlur()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesBlur_Name", "잔상");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesBlur_Description", "피해를 받을 때마다 2턴간 회피 +10(최대 3중첩)");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::Damaged;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	buffs.Add(USpecialForcesBlurBuff::StaticClass());
}

void USpecialForcesBlur::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;
	if (o->IsDead()) return;

	//습득 직후 1회 평가는 기록만 한다
	const int32 hpNow = o->GetHp();
	const bool bDamaged = lastHp >= 0 && hpNow < lastHp;
	lastHp = hpNow;
	if (!bDamaged) return;

	if (UBuffComponent* bc = o->GetBuffComponent())
	{
		for (const TSubclassOf<UBuffBase>& buffClass : buffs)
		{
			bc->AddBuff(buffClass, o);
		}
	}
}
