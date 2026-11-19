//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RiflePreciseShot.generated.h"

//Henry 전용 상급, 사거리와 치명타가 높은 단발 사격
UCLASS()
class PW_API URiflePreciseShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URiflePreciseShot();
};
