//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleSpeedPassive.generated.h"

//공용 중급 강화, 속도가 2 증가한다
UCLASS()
class PW_API UMiddleSpeedPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleSpeedPassive();
};
