//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperRelocate.generated.h"

//상급 랜덤 강화, 적을 처치하면 이동력을 얻는다
UCLASS()
class PW_API USniperRelocate : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperRelocate();

	virtual void Execute_AfterSlay(FVector slainLocation) override;

protected:
	//처치마다 얻는 이동력(m)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	float movingPointGain = 6.f;
};
