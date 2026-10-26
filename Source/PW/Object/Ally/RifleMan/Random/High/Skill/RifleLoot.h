//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleLoot.generated.h"

//상급 랜덤 강화, 적을 처치할 때마다 탄창을 회복한다
UCLASS()
class PW_API URifleLoot : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleLoot();

	virtual void Execute_AfterSlay(FVector slainLocation) override;

protected:
	//처치당 회복할 탄창 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Loot")
	int32 resourceGain = 3;
};
