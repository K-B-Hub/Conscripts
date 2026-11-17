//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperMarksmanship.generated.h"

//레벨 20 고정 강화, 명중이 오른다
UCLASS()
class PW_API USniperMarksmanship : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperMarksmanship();
};
