//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestMoveComplete.generated.h"

//테스트용 이동 완료 패시브
UCLASS()
class PW_API UTestMoveComplete : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestMoveComplete();

	virtual void Execute_Conditional() override;
};
