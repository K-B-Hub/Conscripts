//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesMomentum.generated.h"

//중급 랜덤 강화, 적을 처치하면 회피가 오른다
UCLASS()
class PW_API USpecialForcesMomentum : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesMomentum();

	virtual void Execute_AfterSlay(FVector slainLocation) override;
};
