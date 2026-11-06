//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Fixed/Skill/RifleToxicCoating.h"
#include "Characters/CharacterBase.h"

URifleToxicCoating::URifleToxicCoating()
{
	skillName = NSLOCTEXT("Skill", "RifleToxicCoating_Name", "독성 코팅");
	skillDescription = NSLOCTEXT("Skill", "RifleToxicCoating_Description", "공격으로 피해를 준 대상에게 현재 체력의 20%만큼 추가 피해를 준다(체력을 1 아래로 깎지 않음).");

	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterDamage;
	conditionType = EConditionalType::None;

	bAllowDuplicate = false;
}

void URifleToxicCoating::Execute_AfterDamage(ACharacterBase* target)
{
	if (!target || target->IsDead()) return;

	//지속 피해 유형이라 비치명, 체력 1까지만 깎이고 이 피해로는 죽지 않는다
	//비치명 피해는 AfterDamage를 다시 발동시키지 않아 재귀가 생기지 않는다
	const int32 amount = FMath::Max(1, FMath::RoundToInt(target->GetHp() * hpRatioPercent / 100.f));
	target->ReceiveDamage(amount, false);
}
