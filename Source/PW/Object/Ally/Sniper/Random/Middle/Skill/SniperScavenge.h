//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperScavenge.generated.h"

//중급 랜덤 강화, 적을 처치하면 탄환을 회복한다
UCLASS()
class PW_API USniperScavenge : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperScavenge();

	virtual void Execute_AfterSlay(FVector slainLocation) override;

protected:
	//처치마다 회복할 탄환 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	int32 resourceGain = 1;
};
