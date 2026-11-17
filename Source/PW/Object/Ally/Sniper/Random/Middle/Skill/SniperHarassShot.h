//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperHarassShot.generated.h"

//중급 랜덤 강화, 행동력 1로 가볍게 견제한다
UCLASS()
class PW_API USniperHarassShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperHarassShot();
};
