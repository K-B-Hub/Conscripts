//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighEvasionPassive.generated.h"

//공용 상급 강화, 회피가 10 증가한다
UCLASS()
class PW_API UHighEvasionPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighEvasionPassive();
};
