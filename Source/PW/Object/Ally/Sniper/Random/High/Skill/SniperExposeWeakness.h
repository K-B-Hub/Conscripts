//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperExposeWeakness.generated.h"

//상급 랜덤 강화, 피해를 준 대상이 받는 피해를 늘린다
UCLASS()
class PW_API USniperExposeWeakness : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperExposeWeakness();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;
};
