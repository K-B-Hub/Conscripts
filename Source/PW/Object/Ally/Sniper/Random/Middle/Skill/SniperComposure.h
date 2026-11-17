//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperComposure.generated.h"

//중급 랜덤 강화, 체력에 여유가 있으면 명중이 오른다
UCLASS()
class PW_API USniperComposure : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperComposure();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
