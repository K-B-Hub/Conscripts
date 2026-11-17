//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperTargetMark.generated.h"

//중급 랜덤 강화, 대상에 표식을 남겨 회피를 깎는다
UCLASS()
class PW_API USniperTargetMark : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperTargetMark();
};
