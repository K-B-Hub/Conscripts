//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesBreachShot.generated.h"

//Michael 전용 하급, 파고들며 가까운 적을 쏜다
UCLASS()
class PW_API USpecialForcesBreachShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesBreachShot();
};
