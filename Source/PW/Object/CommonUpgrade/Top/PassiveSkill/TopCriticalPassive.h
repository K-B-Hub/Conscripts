//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopCriticalPassive.generated.h"

//공용 최상급 강화, 치명타가 30 증가한다
UCLASS()
class PW_API UTopCriticalPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopCriticalPassive();
};