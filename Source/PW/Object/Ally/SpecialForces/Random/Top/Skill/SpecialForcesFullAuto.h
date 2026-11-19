//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesFullAuto.generated.h"

//Michael 전용 최상급, 탄창을 비우며 네 발을 쏟아붓는다
UCLASS()
class PW_API USpecialForcesFullAuto : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesFullAuto();
};
