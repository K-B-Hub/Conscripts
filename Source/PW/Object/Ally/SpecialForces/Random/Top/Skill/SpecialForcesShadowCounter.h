//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesShadowCounter.generated.h"

//최상급 랜덤 강화, 공격을 피하면 공격자에게 반격 피해를 준다
//명중·치명 판정 없이 방어만 적용, 적 행동 도중 공격자가 죽지 않도록 비치명
UCLASS()
class PW_API USpecialForcesShadowCounter : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesShadowCounter();

	virtual void Execute_AfterEvade(ACharacterBase* attacker) override;

protected:
	//반격 공격력 계수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float counterRatio = 1.0f;
};
