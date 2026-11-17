//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperConfusionInduce.generated.h"

//상급 랜덤 강화, 피해를 준 적에게 확률로 혼란을 건다
UCLASS()
class PW_API USniperConfusionInduce : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperConfusionInduce();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;

protected:
	//발동 확률(%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	float chancePercent = 20.f;
};
