//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleBladeSharpening.generated.h"

//상급 랜덤 강화, 근접 스킬로 공격할 때 치명이 오른다
UCLASS()
class PW_API URifleBladeSharpening : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleBladeSharpening();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
