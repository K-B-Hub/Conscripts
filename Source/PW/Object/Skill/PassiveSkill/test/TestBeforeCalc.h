//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestBeforeCalc.generated.h"

class ACharacterBase;

//테스트용 피해 계산 전 패시브
UCLASS()
class PW_API UTestBeforeCalc : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestBeforeCalc();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
