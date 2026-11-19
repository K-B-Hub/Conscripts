//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperDoubleSnipe.generated.h"

//저격수 직업 공용 상급, 두 발을 연달아 쏜다
UCLASS()
class PW_API USniperDoubleSnipe : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperDoubleSnipe();
};
