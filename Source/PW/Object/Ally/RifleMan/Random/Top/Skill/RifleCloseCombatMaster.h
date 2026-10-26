//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleCloseCombatMaster.generated.h"

//최상급 랜덤 강화, 근접 스킬로 공격할 때 치명과 주는 피해가 오른다
UCLASS()
class PW_API URifleCloseCombatMaster : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleCloseCombatMaster();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;
};
