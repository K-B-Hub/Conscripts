//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestEnemyDeath.generated.h"

//테스트용 적 사망 패시브
UCLASS()
class PW_API UTestEnemyDeath : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestEnemyDeath();

	virtual void Execute_Conditional() override;
};
