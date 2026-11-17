//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperSteadyAim.generated.h"

//중급 랜덤 강화, 이번 턴에 움직이지 않았으면 명중과 치명이 오른다
UCLASS()
class PW_API USniperSteadyAim : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperSteadyAim();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
