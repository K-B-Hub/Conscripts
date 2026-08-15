//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleMovingPointPassive.generated.h"

//공용 중급 강화, 이동력이 2 증가한다
UCLASS()
class PW_API UMiddleMovingPointPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleMovingPointPassive();
};
