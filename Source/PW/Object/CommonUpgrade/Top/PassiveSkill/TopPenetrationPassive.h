//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopPenetrationPassive.generated.h"

//공용 최상급 강화, 관통력이 25 증가한다
UCLASS()
class PW_API UTopPenetrationPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopPenetrationPassive();
};