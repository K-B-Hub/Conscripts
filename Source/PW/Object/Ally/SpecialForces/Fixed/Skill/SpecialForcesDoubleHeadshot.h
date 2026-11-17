//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesDoubleHeadshot.generated.h"

//레벨 60 고정 강화, 두 발을 머리에 꽂는다
UCLASS()
class PW_API USpecialForcesDoubleHeadshot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesDoubleHeadshot();
};
