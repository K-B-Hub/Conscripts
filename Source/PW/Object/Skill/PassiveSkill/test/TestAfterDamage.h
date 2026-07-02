//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestAfterDamage.generated.h"

class ACharacterBase;

//테스트용 피해 후 패시브
UCLASS()
class PW_API UTestAfterDamage : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestAfterDamage();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;
};
