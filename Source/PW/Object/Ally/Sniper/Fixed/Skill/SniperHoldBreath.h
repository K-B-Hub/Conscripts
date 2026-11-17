//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "SniperHoldBreath.generated.h"

//레벨 35 고정 강화, 숨을 참고 다음 사격을 준비한다
//쓴 턴은 행동력이 남지 않아 다음 턴에 쏘게 된다
UCLASS()
class PW_API USniperHoldBreath : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	USniperHoldBreath();
};
