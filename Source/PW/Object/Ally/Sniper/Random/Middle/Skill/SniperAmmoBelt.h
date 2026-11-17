//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperAmmoBelt.generated.h"

//중급 랜덤 강화, 탄환 최대치가 늘어난다
UCLASS()
class PW_API USniperAmmoBelt : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperAmmoBelt();
};
