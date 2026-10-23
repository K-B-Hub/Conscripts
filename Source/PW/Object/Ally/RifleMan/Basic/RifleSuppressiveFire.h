//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleSuppressiveFire.generated.h"

//소총수 기본 스킬, 한 명에게 사격한다. 탄창을 소모하는 주력 공격
UCLASS()
class PW_API URifleSuppressiveFire : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleSuppressiveFire();
};
