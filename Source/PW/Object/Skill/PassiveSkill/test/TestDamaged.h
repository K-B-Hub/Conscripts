//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestDamaged.generated.h"

//테스트용 피격 패시브
UCLASS()
class PW_API UTestDamaged : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestDamaged();

	virtual void Execute_Conditional() override;
};
