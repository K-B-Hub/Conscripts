//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighAccuracyPassive.generated.h"

//공용 상급 강화, 명중이 10 증가한다
UCLASS()
class PW_API UHighAccuracyPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighAccuracyPassive();
};
