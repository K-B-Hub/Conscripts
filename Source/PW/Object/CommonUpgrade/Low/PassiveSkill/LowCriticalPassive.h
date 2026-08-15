//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "LowCriticalPassive.generated.h"

//공용 하급 강화, 치명타가 3 증가한다
UCLASS()
class PW_API ULowCriticalPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	ULowCriticalPassive();
};
