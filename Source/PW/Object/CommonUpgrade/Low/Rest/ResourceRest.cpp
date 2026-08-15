//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/CommonUpgrade/Low/Rest/ResourceRest.h"
#include "Characters/CharacterBase.h"

UResourceRest::UResourceRest()
{
	skillName = NSLOCTEXT("Rest", "Resource_Name", "하급 전투 자원 회복");
	skillDescription = NSLOCTEXT("Rest", "Resource_Description", "즉시 아군 전원의 전투 자원을 최대치의 30%만큼 회복한다");
}

void UResourceRest::Execute(ACharacterBase* instigator)
{
	Super::Execute(instigator);

	TArray<ACharacterBase*> team;
	CollectTeam(instigator, team);

	//회복량은 각자의 최대 전투 자원 기준, 초과분은 GainBattleResource가 클램프
	for (ACharacterBase* character : team)
	{
		character->GainBattleResource(CalcRatioAmount(character->GetMaxBattleResource(), resourceRatio));
	}
}