//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesSuppressStrike.generated.h"

//중급 랜덤 강화, 개머리판으로 쳐서 밀쳐낸다
UCLASS()
class PW_API USpecialForcesSuppressStrike : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesSuppressStrike();
};
