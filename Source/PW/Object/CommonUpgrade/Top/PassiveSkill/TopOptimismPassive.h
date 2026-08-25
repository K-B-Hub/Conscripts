//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TopOptimismPassive.generated.h"

//공용 최상급 강화, 스트레스 이벤트가 항상 긍정 풀에서 추첨된다
UCLASS()
class PW_API UTopOptimismPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTopOptimismPassive();
};