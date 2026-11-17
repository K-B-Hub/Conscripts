//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperNest.generated.h"

//상급 랜덤 강화, 포복 중에 공격하면 명중, 치명, 주는 피해가 오른다
UCLASS()
class PW_API USniperNest : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperNest();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
