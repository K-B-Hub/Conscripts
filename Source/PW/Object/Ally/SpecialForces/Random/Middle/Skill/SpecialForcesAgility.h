//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesAgility.generated.h"

//중급 랜덤 강화, 주변 적 수만큼 회피가 오른다
//1중첩 = CDO의 evasion 3, 중첩 수만큼 스탯 델타를 반복 적용한다
UCLASS()
class PW_API USpecialForcesAgility : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesAgility();

	virtual void Execute_Conditional() override;

protected:
	//적을 세는 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float searchRange = 4.f * CmPerMeter;
	//최대 중첩
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	int32 maxStack = 5;

private:
	//현재 적용된 중첩 수, 차이만큼만 가감해 중복 적용을 막는다
	UPROPERTY()
	int32 appliedStack = 0;
};
