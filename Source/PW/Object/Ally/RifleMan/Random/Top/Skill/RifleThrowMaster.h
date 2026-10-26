//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleThrowMaster.generated.h"

//최상급 랜덤 강화, 투척 스킬의 광역 치명타 봉쇄를 푼다
//판정은 SetCalcedStats와 DispatchBeforeDamageCalc가 플래그를 보고 처리한다
UCLASS()
class PW_API URifleThrowMaster : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleThrowMaster();
};
