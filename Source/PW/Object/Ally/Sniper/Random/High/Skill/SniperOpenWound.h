//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperOpenWound.generated.h"

//상급 랜덤 강화, 피해를 준 대상에게 출혈을 남긴다
UCLASS()
class PW_API USniperOpenWound : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperOpenWound();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;
};
