//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesAssassinPrep.generated.h"

//레벨 35 고정 강화, 몸을 감추고 급소를 노릴 준비를 한다
UCLASS()
class PW_API USpecialForcesAssassinPrep : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesAssassinPrep();
};
