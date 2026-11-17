//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperIncapacitate.generated.h"

//최상급 랜덤 강화, 급소를 쏴 행동력을 빼앗는다
UCLASS()
class PW_API USniperIncapacitate : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperIncapacitate();
};
