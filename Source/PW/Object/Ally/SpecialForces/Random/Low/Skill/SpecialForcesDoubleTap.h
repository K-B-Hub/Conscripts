//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesDoubleTap.generated.h"

//Michael 시그니처, 두 발을 빠르게 연달아 쏜다
UCLASS()
class PW_API USpecialForcesDoubleTap : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesDoubleTap();
};
