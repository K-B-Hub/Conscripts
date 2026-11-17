//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesAfterimage.generated.h"

//중급 랜덤 강화, 공격을 피하면 이동력이 오른다(최대 3중첩)
UCLASS()
class PW_API USpecialForcesAfterimage : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesAfterimage();

	virtual void Execute_AfterEvade(ACharacterBase* attacker) override;
};
