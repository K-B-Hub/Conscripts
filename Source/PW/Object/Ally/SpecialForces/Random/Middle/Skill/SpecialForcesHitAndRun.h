//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesHitAndRun.generated.h"

//중급 랜덤 강화, 치명타를 맞히면 이동력을 얻는다
UCLASS()
class PW_API USpecialForcesHitAndRun : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesHitAndRun();

	virtual void Execute_AfterCritical(ACharacterBase* target, int32 dealtDamage) override;

protected:
	//치명타마다 얻는 이동력(m)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float movingPointGain = 3.f;
};
