//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestStatPassive.generated.h"

//테스트용 스탯 패시브
UCLASS()
class PW_API UTestStatPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestStatPassive();
};
