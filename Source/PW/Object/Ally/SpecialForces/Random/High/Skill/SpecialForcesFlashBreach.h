//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesFlashBreach.generated.h"

//상급 랜덤 강화, 섬광탄을 굴려 넣고 돌입한다
UCLASS()
class PW_API USpecialForcesFlashBreach : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesFlashBreach();
};
