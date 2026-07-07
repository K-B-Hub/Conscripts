//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SelfBuffSkill.generated.h"

//테스트용 자기 버프 스킬, UtilityAI Self 후보 검증용
UCLASS()
class PW_API USelfBuffSkill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USelfBuffSkill();
};
