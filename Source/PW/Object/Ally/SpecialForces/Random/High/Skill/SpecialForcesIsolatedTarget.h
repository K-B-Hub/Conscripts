//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesIsolatedTarget.generated.h"

//상급 랜덤 강화, 홀로 떨어진 적을 쏠 때 치명과 주는 피해가 오른다
UCLASS()
class PW_API USpecialForcesIsolatedTarget : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesIsolatedTarget();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//고립 판정 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float isolationRange = 8.f * CmPerMeter;
};
