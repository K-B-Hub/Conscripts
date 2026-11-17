//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesSwiftResupply.generated.h"

//중급 랜덤 강화, 장비 정비의 행동력 소모가 줄어든다
UCLASS()
class PW_API USpecialForcesSwiftResupply : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesSwiftResupply();
};
