//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "LowHpPassive.generated.h"

//공용 하급 강화, 최대 체력이 2 증가한다
UCLASS()
class PW_API ULowHpPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	ULowHpPassive();
};
