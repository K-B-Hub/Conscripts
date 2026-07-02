//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestBeforeMove.generated.h"

//테스트용 이동 전 패시브
UCLASS()
class PW_API UTestBeforeMove : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestBeforeMove();

	virtual void Execute_BeforeMove(bool bIsMoved, int32 sign) override;
};
