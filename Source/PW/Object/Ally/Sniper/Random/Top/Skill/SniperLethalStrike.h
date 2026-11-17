//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperLethalStrike.generated.h"

//최상급 랜덤 강화, 치명타 판정에 실패하면 한 번 더 판정한다
//재굴림은 BeforeDamageCalc 디스패치가 확률로 환산해 예측값과 실제 판정에 함께 반영한다
UCLASS()
class PW_API USniperLethalStrike : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperLethalStrike();
};
