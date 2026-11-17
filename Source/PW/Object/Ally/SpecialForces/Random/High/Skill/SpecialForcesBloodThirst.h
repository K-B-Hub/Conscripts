//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesBloodThirst.generated.h"

//상급 랜덤 강화, 치명타로 준 피해의 일부를 회복한다
UCLASS()
class PW_API USpecialForcesBloodThirst : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesBloodThirst();

	virtual void Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage) override;

protected:
	//회복량, 준 피해 대비(%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 lifestealPercent = 30;
};
