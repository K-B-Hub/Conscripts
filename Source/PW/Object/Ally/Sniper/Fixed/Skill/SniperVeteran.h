//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperVeteran.generated.h"

//레벨 50 고정 강화, 턴 시작마다 탄환을 회복한다
UCLASS()
class PW_API USniperVeteran : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperVeteran();

	virtual void Execute_Conditional() override;

protected:
	//턴 시작마다 회복할 탄환 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	int32 resourceGain = 1;

private:
	//명중 보너스를 이미 걸었는지, 첫 턴 시작에 한 번만 건다
	UPROPERTY()
	bool bApplied = false;
};
