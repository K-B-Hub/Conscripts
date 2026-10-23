//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleCoveringFire.generated.h"

//레벨 25 고정 강화, 아군을 지정해 그 주변의 적들을 사격한다
//지정 대상은 아군이지만 피해를 받는 것은 범위 안의 적뿐이다
UCLASS()
class PW_API URifleCoveringFire : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleCoveringFire();
};
