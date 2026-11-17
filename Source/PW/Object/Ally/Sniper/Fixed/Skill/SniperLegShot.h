//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperLegShot.generated.h"

//레벨 10 고정 강화, 다리를 쏴 이동력을 묶는다
UCLASS()
class PW_API USniperLegShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperLegShot();
};
