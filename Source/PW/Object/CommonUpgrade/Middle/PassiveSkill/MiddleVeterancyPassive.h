//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleVeterancyPassive.generated.h"

//공용 중급 강화, 8m 밖의 적을 공격할 때 공격력이 3, 명중이 10 증가한다
UCLASS()
class PW_API UMiddleVeterancyPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleVeterancyPassive();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType, const FVector& attackerLocation) override;

protected:
	//발동 거리, 800 = 8m 초과부터 적용
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	float rangedThreshold = 800.f;
};