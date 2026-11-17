//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperShoulderShot.generated.h"

//상급 랜덤 강화, 어깨를 쏴 명중과 치명을 떨어뜨린다
UCLASS()
class PW_API USniperShoulderShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperShoulderShot();
};
