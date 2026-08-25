//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighCriticalDamagePassive.generated.h"

//공용 상급 강화, 치명타 피해 배율이 1.0 증가한다
UCLASS()
class PW_API UHighCriticalDamagePassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighCriticalDamagePassive();
};