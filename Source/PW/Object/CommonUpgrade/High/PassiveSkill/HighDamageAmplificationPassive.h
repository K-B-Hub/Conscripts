//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighDamageAmplificationPassive.generated.h"

//공용 상급 강화, 주는 피해량 증가 수치가 6 증가한다
UCLASS()
class PW_API UHighDamageAmplificationPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighDamageAmplificationPassive();
};
