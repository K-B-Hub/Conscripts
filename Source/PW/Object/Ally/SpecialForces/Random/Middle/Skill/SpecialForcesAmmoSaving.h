//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesAmmoSaving.generated.h"

//중급 랜덤 강화, 치명타를 맞히면 장비를 회복한다
UCLASS()
class PW_API USpecialForcesAmmoSaving : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesAmmoSaving();

	virtual void Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage) override;

protected:
	//한 번에 회복할 장비 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 resourceGain = 1;
};
