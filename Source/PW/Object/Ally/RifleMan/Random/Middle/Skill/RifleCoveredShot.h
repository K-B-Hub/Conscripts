//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleCoveredShot.generated.h"

//중급 랜덤 강화, 포복 중에 공격하면 명중과 치명이 오른다
UCLASS()
class PW_API URifleCoveredShot : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleCoveredShot();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};