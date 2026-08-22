//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighPenetrationPassive.generated.h"

//공용 상급 강화, 관통력이 6 증가한다
UCLASS()
class PW_API UHighPenetrationPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighPenetrationPassive();
};
