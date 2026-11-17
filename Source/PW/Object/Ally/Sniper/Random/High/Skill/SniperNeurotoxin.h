//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperNeurotoxin.generated.h"

//상급 랜덤 강화, 피해를 준 대상의 이동력을 깎는다
UCLASS()
class PW_API USniperNeurotoxin : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperNeurotoxin();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;
};
