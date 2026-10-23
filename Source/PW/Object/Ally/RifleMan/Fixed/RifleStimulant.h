//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleStimulant.generated.h"

//레벨 35 고정 강화, 자신에게 각성제를 투여해 1턴간 강화된다
//부여할 버프는 BP의 buffs 배열에서 URifleStimulantBuff 파생을 지정한다
UCLASS()
class PW_API URifleStimulant : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleStimulant();
};
