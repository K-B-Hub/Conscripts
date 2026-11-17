//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SniperStealthShooter.generated.h"

//중급 랜덤 강화, 주변에 적이 없으면 치명이 오른다
UCLASS()
class PW_API USniperStealthShooter : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USniperStealthShooter();

	virtual void Execute_Conditional() override;

protected:
	//적을 찾는 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Sniper")
	float searchRange = 8.f * CmPerMeter;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};
