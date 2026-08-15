//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddlePenetrationPassive.generated.h"

//공용 중급 강화, 관통력이 3 증가한다
UCLASS()
class PW_API UMiddlePenetrationPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddlePenetrationPassive();
};
