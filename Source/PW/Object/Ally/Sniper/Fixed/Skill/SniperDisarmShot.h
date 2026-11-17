//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperDisarmShot.generated.h"

//레벨 15 고정 강화, 총을 쥔 손을 쏴 명중을 떨어뜨린다
UCLASS()
class PW_API USniperDisarmShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperDisarmShot();
};
