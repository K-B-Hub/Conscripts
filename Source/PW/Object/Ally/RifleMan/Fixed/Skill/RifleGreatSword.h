//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleGreatSword.generated.h"

//레벨 10 고정 강화, 총검술보다 위력이 높은 근접 공격
UCLASS()
class PW_API URifleGreatSword : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleGreatSword();
};
