//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperBleedShot.generated.h"

//중급 랜덤 강화, 맞은 대상에게 출혈을 남긴다
UCLASS()
class PW_API USniperBleedShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperBleedShot();
};
