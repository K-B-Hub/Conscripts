//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopEvasionPassive.generated.h"

//공용 최상급 강화, 회피가 35 증가한다
UCLASS()
class PW_API UTopEvasionPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopEvasionPassive();
};