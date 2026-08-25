//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopSightPassive.generated.h"

//공용 최상급 강화, 시야가 20 증가한다
UCLASS()
class PW_API UTopSightPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopSightPassive();
};