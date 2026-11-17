//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperLongRangePrecision.generated.h"

//중급 랜덤 강화, 먼 적을 쏠 때 주는 피해가 늘어난다
UCLASS()
class PW_API USniperLongRangePrecision : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperLongRangePrecision();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//발동 거리, 이상부터 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	float farRange = 15.f * CmPerMeter;
};
