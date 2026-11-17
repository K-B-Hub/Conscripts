//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperHeadshot.generated.h"

//레벨 60 고정 강화, 머리를 노려 치명타를 노린다
UCLASS()
class PW_API USniperHeadshot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperHeadshot();
};
