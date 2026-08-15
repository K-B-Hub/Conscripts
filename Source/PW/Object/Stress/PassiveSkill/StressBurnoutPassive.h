//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "StressBurnoutPassive.generated.h"

//스트레스 부정 이벤트, 정신력 영구 감소(중복 획득 가능)
UCLASS()
class PW_API UStressBurnoutPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UStressBurnoutPassive();
};