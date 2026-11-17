//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesCQBExpert.generated.h"

//레벨 70 고정 강화, 가까운 적에게 주는 피해가 늘어난다
UCLASS()
class PW_API USpecialForcesCQBExpert : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesCQBExpert();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//발동 거리, 이내부터 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float closeRange = 6.f * CmPerMeter;
};
