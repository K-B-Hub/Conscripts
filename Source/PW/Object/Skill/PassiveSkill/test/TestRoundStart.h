//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestRoundStart.generated.h"

//테스트용 라운드 시작 패시브
UCLASS()
class PW_API UTestRoundStart : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestRoundStart();

	virtual void Execute_Conditional() override;
};
