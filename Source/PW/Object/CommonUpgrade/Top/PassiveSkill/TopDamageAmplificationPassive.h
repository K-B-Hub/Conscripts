//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopDamageAmplificationPassive.generated.h"

//공용 최상급 강화, 주는 피해량 증가 수치가 20 증가한다
UCLASS()
class PW_API UTopDamageAmplificationPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopDamageAmplificationPassive();
};