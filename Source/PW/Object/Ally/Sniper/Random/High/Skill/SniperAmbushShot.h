//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperAmbushShot.generated.h"

//상급 랜덤 강화, 체력이 가득 찬 적에게 주는 피해가 늘어난다
UCLASS()
class PW_API USniperAmbushShot : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperAmbushShot();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
