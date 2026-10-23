//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleAimedShot.generated.h"

//레벨 5 고정 강화, 견제 사격보다 위력과 명중이 높은 조준 사격
UCLASS()
class PW_API URifleAimedShot : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleAimedShot();
};
