//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleBurstFire.generated.h"

//레벨 60 고정 강화, 3회 연속 사격
//멀티픽으로 구현하며 같은 대상을 세 번 고르면 그 대상이 세 번 맞는다
UCLASS()
class PW_API URifleBurstFire : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleBurstFire();
};
