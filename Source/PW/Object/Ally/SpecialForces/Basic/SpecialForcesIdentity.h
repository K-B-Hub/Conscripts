//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesIdentity.generated.h"

//특수부대원 기본 패시브, 주변에 아군이 없으면 회피와 명중이 오른다
UCLASS()
class PW_API USpecialForcesIdentity : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesIdentity();

	virtual void Execute_Conditional() override;

protected:
	//고립 판정 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|SpecialForces")
	float isolationRange = 3.f * CmPerMeter;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};
