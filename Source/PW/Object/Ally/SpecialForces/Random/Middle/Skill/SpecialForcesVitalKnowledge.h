//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesVitalKnowledge.generated.h"

//중급 랜덤 강화, 체력이 가득 찬 적을 쏠 때 치명이 오른다
UCLASS()
class PW_API USpecialForcesVitalKnowledge : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesVitalKnowledge();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
