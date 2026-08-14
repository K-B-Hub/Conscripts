//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Rest/HealRest.h"
#include "Characters/CharacterBase.h"

UHealRest::UHealRest()
{
	skillName = NSLOCTEXT("Rest", "Heal_Name", "하급 체력 회복");
	skillDescription = NSLOCTEXT("Rest", "Heal_Description", "즉시 아군 전원의 체력을 최대 체력의 30%만큼 회복한다");
}

void UHealRest::Execute(ACharacterBase* instigator)
{
	Super::Execute(instigator);

	TArray<ACharacterBase*> team;
	CollectTeam(instigator, team);

	//회복량은 각자의 최대 체력 기준, 만피면 ReceiveDamage가 클램프
	for (ACharacterBase* character : team)
	{
		const int32 amount = CalcRatioAmount(character->GetMaxHp(), healRatio);
		if (amount <= 0) continue;

		//음수 전달이 회복, 회복 스킬과 동일하게 bIsLethal=true
		character->ReceiveDamage(-amount, true);
	}
}