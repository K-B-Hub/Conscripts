//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperIdentity.generated.h"

//저격수 기본 패시브, 가까운 적을 쏘면 명중과 치명이 깎인다
UCLASS()
class PW_API USniperIdentity : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperIdentity();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//패널티 거리, 이내부터 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	float closeRange = 4.f * CmPerMeter;
};
