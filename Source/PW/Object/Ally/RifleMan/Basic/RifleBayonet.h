//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleBayonet.generated.h"

//소총수 기본 스킬, 총검으로 근접한 대상을 찌른다
UCLASS()
class PW_API URifleBayonet : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleBayonet();
};
