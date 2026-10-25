//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleComradeTrust.generated.h"

//중급 랜덤 강화, 주변 아군 수만큼 자신의 공격력이 오른다
//1스택 = CDO의 atk 1, 스택 수만큼 스탯 델타를 반복 적용한다
UCLASS()
class PW_API URifleComradeTrust : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleComradeTrust();

	virtual void Execute_Conditional() override;

protected:
	//아군을 세는 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Aura")
	float auraRange = 5.f * CmPerMeter;

	//최대 스택
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Aura")
	int32 maxStack = 10;

private:
	//현재 적용된 스택 수, 차이만큼만 가감해 중복 적용을 막는다
	UPROPERTY()
	int32 appliedStack = 0;
};