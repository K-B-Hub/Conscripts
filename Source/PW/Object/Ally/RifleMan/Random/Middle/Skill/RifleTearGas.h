//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleTearGas.generated.h"

//중급 랜덤 강화, 착지 지점 범위의 모든 캐릭터에게 지속 피해를 남긴다
//부여할 상태이상은 BP의 ailments 배열에서 URifleTearGasAilment 파생을 지정한다
UCLASS()
class PW_API URifleTearGas : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleTearGas();
};