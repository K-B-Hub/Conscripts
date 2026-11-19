//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperAimedSnipe.generated.h"

//Thomas 시그니처, 더 오래 조준해 강하게 쏜다
UCLASS()
class PW_API USniperAimedSnipe : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperAimedSnipe();
};
