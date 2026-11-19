//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesDoubleHeadshot.generated.h"

//Michael 전용 상급, 두 발을 머리에 꽂는다
UCLASS()
class PW_API USpecialForcesDoubleHeadshot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesDoubleHeadshot();
};
