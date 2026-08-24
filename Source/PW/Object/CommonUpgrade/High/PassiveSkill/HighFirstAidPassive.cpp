//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/High/PassiveSkill/HighFirstAidPassive.h"
#include "Characters/CharacterBase.h"

UHighFirstAidPassive::UHighFirstAidPassive()
{
	skillName = NSLOCTEXT("Skill", "HighFirstAid_Name", "응급 처치");
	skillDescription = NSLOCTEXT("Skill", "HighFirstAid_Description", "턴이 시작할 때 체력을 10% 회복한다");

	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::TurnStart;

	//고유 강화라 중복 획득 불가
	bAllowDuplicate = false;
}

void UHighFirstAidPassive::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	//반올림 후 최소 1 보장, 음수 전달이 회복이며 만피면 ReceiveDamage가 클램프
	const int32 amount = FMath::Max(1, FMath::RoundToInt(o->GetMaxHp() * healRatio));
	o->ReceiveDamage(-amount, true);
}
