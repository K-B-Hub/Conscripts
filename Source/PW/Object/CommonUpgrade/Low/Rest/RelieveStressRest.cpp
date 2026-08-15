//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Low/Rest/RelieveStressRest.h"
#include "Characters/CharacterBase.h"

URelieveStressRest::URelieveStressRest()
{
	skillName = NSLOCTEXT("Rest", "RelieveStress_Name", "하급 스트레스 회복");
	skillDescription = NSLOCTEXT("Rest", "RelieveStress_Description", "즉시 아군 전원의 스트레스를 최대치의 30%만큼 감소시킨다");
}

void URelieveStressRest::Execute(ACharacterBase* instigator)
{
	Super::Execute(instigator);

	TArray<ACharacterBase*> team;
	CollectTeam(instigator, team);

	//감소량은 각자의 최대 스트레스 기준, 아군 외 대상은 RelieveStress가 무시
	for (ACharacterBase* character : team)
	{
		character->RelieveStress(CalcRatioAmount(character->GetMaxStress(), relieveRatio));
	}
}