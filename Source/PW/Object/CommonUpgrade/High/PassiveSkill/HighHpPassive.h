//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighHpPassive.generated.h"

//공용 상급 강화, 최대 체력이 5 증가한다
UCLASS()
class PW_API UHighHpPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighHpPassive();
};
