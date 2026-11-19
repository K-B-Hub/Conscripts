//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesPointBlankHeadshot.generated.h"

//Jack 시그니처, 코앞의 적 머리를 노린다
UCLASS()
class PW_API USpecialForcesPointBlankHeadshot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesPointBlankHeadshot();
};
