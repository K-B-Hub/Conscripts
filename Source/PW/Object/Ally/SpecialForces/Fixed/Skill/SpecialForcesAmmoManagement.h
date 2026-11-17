//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesAmmoManagement.generated.h"

//레벨 50 고정 강화, 턴 시작마다 장비를 회복한다
UCLASS()
class PW_API USpecialForcesAmmoManagement : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesAmmoManagement();

	virtual void Execute_Conditional() override;

protected:
	//한 번에 회복할 장비 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 resourceGain = 1;
};
