//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesCarbineShot.generated.h"

//특수부대원 기본 스킬, 소음 카빈으로 1명을 쏜다
UCLASS()
class PW_API USpecialForcesCarbineShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesCarbineShot();
};
