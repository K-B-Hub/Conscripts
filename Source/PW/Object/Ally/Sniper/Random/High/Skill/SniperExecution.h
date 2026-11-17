//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperExecution.generated.h"

//상급 랜덤 강화, 쓰러져 가는 적에게 주는 피해가 크게 늘어난다
UCLASS()
class PW_API USniperExecution : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperExecution();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
