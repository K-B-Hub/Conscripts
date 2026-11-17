//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperOneShotOneKill.generated.h"

//상급 랜덤 강화, 약해진 적을 원거리로 쏠 때 주는 피해와 치명이 오른다
UCLASS()
class PW_API USniperOneShotOneKill : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperOneShotOneKill();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
