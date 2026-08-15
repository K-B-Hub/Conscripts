//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleHpPassive.generated.h"

//공용 중급 강화, 최대 체력이 3 증가한다
UCLASS()
class PW_API UMiddleHpPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleHpPassive();
};
