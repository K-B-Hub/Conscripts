//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesCloseMarksmanship.generated.h"

//중급 랜덤 강화, 가까운 적에게 치명타 피해가 오른다
UCLASS()
class PW_API USpecialForcesCloseMarksmanship : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesCloseMarksmanship();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//발동 거리, 이내부터 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float closeRange = 4.f * CmPerMeter;
};
