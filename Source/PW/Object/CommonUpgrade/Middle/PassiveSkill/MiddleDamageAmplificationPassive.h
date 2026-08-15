//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleDamageAmplificationPassive.generated.h"

//공용 중급 강화, 주는 피해량이 3 증가한다
UCLASS()
class PW_API UMiddleDamageAmplificationPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleDamageAmplificationPassive();
};
