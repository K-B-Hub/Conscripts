//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/CommonUpgrade/Middle/Rest/ReinforcementRest.h"
#include "Characters/AllyCharacterBase.h"
#include "PlayerController/BattleController.h"

void UReinforcementRest::Execute(ACharacterBase* instigator)
{
	AAllyCharacterBase* caller = Cast<AAllyCharacterBase>(instigator);
	UWorld* world = caller ? caller->GetWorld() : nullptr;
	if (!world) return;

	if (ABattleController* controller = Cast<ABattleController>(world->GetFirstPlayerController()))
	{
		controller->RequestReinforcement(caller, recruitCount);
	}
}