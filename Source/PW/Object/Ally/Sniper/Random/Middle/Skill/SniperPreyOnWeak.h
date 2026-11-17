//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperPreyOnWeak.generated.h"

//중급 랜덤 강화, 약해진 적을 쏠 때 치명이 오른다
UCLASS()
class PW_API USniperPreyOnWeak : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperPreyOnWeak();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
