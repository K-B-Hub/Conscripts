//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesLoot.generated.h"

//중급 랜덤 강화, 적을 처치하면 장비를 회복한다
UCLASS()
class PW_API USpecialForcesLoot : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesLoot();

	virtual void Execute_AfterSlay(FVector slainLocation) override;

protected:
	//한 번에 회복할 장비 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 resourceGain = 1;
};
