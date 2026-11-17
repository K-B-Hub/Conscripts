//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesDistraction.generated.h"

//중급 랜덤 강화, 엉뚱한 곳을 쏴 대상의 명중을 떨어뜨린다
UCLASS()
class PW_API USpecialForcesDistraction : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesDistraction();
};
