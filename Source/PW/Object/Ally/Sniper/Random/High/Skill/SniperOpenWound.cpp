//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Random/High/Skill/SniperOpenWound.h"
#include "Object/Ally/Sniper/Random/High/Ailment/SniperOpenWoundAilment.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/AilmentComponent.h"
#include "Object/Ailment/AilmentBase.h"

USniperOpenWound::USniperOpenWound()
{
	skillName = NSLOCTEXT("Skill", "SniperOpenWound_Name", "상처 헤집기");
	skillDescription = NSLOCTEXT("Skill", "SniperOpenWound_Description", "공격으로 피해를 준 대상에게 2턴간 턴 시작마다 최대 체력의 10% 피해를 주는 출혈을 부여한다(체력을 1 아래로 깎지 않음).");
	skillGrade = EUpgradeGrade::High;

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;

	//조건부 발동이라 중복 획득 불가
	bAllowDuplicate = false;

	//발동 시 부여, BP에서 아이콘이 붙은 BP 파생으로 교체할 수 있다
	ailments.Add(USniperOpenWoundAilment::StaticClass());
}

void USniperOpenWound::Execute_AfterDamage(ACharacterBase* target)
{
	if (!target || target->IsDead()) return;
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//광역에 휩쓸린 아군은 제외
	if (o->IsAlly() == target->IsAlly()) return;

	if (UAilmentComponent* ac = target->GetAilmentComponent())
	{
		for (const TSubclassOf<UAilmentBase>& ailmentClass : ailments)
		{
			ac->AddAilment(ailmentClass, o);
		}
	}
}
