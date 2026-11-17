//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperReaper.generated.h"

//최상급 랜덤 강화, 적을 처치하면 행동력을 회복한다
UCLASS()
class PW_API USniperReaper : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperReaper();

	virtual void Execute_AfterSlay(FVector slainLocation) override;

protected:
	//처치마다 회복할 행동력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	int32 actionPointGain = 1;
};
