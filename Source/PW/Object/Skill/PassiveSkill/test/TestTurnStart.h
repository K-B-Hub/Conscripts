//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestTurnStart.generated.h"

//테스트용 턴 시작 패시브
UCLASS()
class PW_API UTestTurnStart : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestTurnStart();

	virtual void Execute_Conditional() override;
};
