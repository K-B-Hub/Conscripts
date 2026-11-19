//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleGrenade.generated.h"

//Walter 시그니처, 지면에 수류탄을 던져 착지점 범위에 변동 피해를 준다
UCLASS()
class PW_API URifleGrenade : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleGrenade();
};
