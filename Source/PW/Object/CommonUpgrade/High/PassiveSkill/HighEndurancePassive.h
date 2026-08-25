//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighEndurancePassive.generated.h"

//공용 상급 강화, 체력 30% 이상에서 치사 피해를 받아도 체력 1로 생존한다
UCLASS()
class PW_API UHighEndurancePassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighEndurancePassive();

	virtual bool Execute_BeforeDeath(int32 hpBefore, int32 incomingDamage) override;

protected:
	//생존 가능한 최소 체력 비율(%), 피격 직전 체력 기준
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	int32 hpThresholdPercent = 30;
};