//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleCheekWeld.generated.h"

//중급 랜덤 강화, 이동하지 않은 턴에 공격하면 명중이 오른다
UCLASS()
class PW_API URifleCheekWeld : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleCheekWeld();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};