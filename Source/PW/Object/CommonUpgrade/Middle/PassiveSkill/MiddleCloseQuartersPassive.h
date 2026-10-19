//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleCloseQuartersPassive.generated.h"

//공용 중급 강화, 2m 이내의 적을 공격할 때 공격력이 3, 치명타가 10 증가한다
UCLASS()
class PW_API UMiddleCloseQuartersPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleCloseQuartersPassive();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//발동 거리(2m)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	float meleeRange = 2.f * CmPerMeter;
};