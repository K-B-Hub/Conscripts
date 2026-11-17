//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperShot.generated.h"

//저격수 기본 스킬, 조준경으로 1명을 노려 쏜다
UCLASS()
class PW_API USniperShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperShot();
};
