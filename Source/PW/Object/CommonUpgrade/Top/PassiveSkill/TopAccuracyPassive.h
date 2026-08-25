//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopAccuracyPassive.generated.h"

//공용 최상급 강화, 명중이 40 증가한다
UCLASS()
class PW_API UTopAccuracyPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopAccuracyPassive();
};