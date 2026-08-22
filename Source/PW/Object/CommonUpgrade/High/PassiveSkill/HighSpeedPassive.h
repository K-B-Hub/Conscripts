//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighSpeedPassive.generated.h"

//공용 상급 강화, 속도가 3 증가한다
UCLASS()
class PW_API UHighSpeedPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighSpeedPassive();
};
