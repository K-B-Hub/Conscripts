//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SpecialForcesSilentKill.generated.h"

//중급 랜덤 강화, 장비 없이 소리 없이 처리한다
UCLASS()
class PW_API USpecialForcesSilentKill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesSilentKill();
};
