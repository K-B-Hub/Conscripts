//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperMarksmanship.generated.h"

//미등록(단순 스탯), 명중이 오른다
UCLASS()
class PW_API USniperMarksmanship : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperMarksmanship();
};
