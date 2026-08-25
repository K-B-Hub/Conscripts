//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopActionPointPassive.generated.h"

//공용 최상급 강화, 행동력이 2 증가한다
UCLASS()
class PW_API UTopActionPointPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopActionPointPassive();
};