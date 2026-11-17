//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesExecutioner.generated.h"

//최상급 랜덤 강화, 치명타를 맞히면 행동력을 회복한다(라운드당 1회)
UCLASS()
class PW_API USpecialForcesExecutioner : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesExecutioner();

	virtual void Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage) override;

protected:
	//회복할 행동력
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 actionPointGain = 1;

private:
	//마지막으로 발동한 라운드, 라운드당 1회 래치
	UPROPERTY()
	int32 triggeredRound = -1;
};
