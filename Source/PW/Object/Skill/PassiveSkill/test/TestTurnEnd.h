//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestTurnEnd.generated.h"

//테스트용 턴 종료 패시브
UCLASS()
class PW_API UTestTurnEnd : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestTurnEnd();

	virtual void Execute_Conditional() override;
};
