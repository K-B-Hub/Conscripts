//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperSwiftReload.generated.h"

//중급 랜덤 강화, 재장전의 행동력 소모가 줄어든다
UCLASS()
class PW_API USniperSwiftReload : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperSwiftReload();
};
