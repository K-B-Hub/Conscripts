//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleCloseCombat.generated.h"

//상급 랜덤 강화, 근접 스킬의 행동력 소모를 1 줄인다
//할인은 UActiveSkillBase::GetActionPointCost가 조회하므로 실행 훅이 없다
UCLASS()
class PW_API URifleCloseCombat : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleCloseCombat();
};
