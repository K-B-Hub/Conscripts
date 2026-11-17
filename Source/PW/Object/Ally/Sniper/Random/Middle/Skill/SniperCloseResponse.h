//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperCloseResponse.generated.h"

//중급 랜덤 강화, 가까운 적을 쏠 때 명중이 오른다
UCLASS()
class PW_API USniperCloseResponse : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperCloseResponse();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//발동 거리, 이내부터 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	float closeRange = 4.f * CmPerMeter;
};
