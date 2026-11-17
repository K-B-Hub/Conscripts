//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesAdrenaline.generated.h"

//중급 랜덤 강화, 공격을 피하면 체력을 회복한다
UCLASS()
class PW_API USpecialForcesAdrenaline : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesAdrenaline();

	virtual void Execute_AfterEvade(ACharacterBase* attacker) override;

protected:
	//회복량, 최대 체력 대비(%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 healPercent = 10;
};
