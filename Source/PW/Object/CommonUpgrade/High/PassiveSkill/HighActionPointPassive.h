//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighActionPointPassive.generated.h"

//공용 상급 강화, 행동력이 1 증가한다
UCLASS()
class PW_API UHighActionPointPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighActionPointPassive();
};
