//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "LowEvasionPassive.generated.h"

//공용 하급 강화, 회피가 3 증가한다
UCLASS()
class PW_API ULowEvasionPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	ULowEvasionPassive();
};
