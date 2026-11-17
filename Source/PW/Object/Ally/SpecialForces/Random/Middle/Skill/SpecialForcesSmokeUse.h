//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "SpecialForcesSmokeUse.generated.h"

//중급 랜덤 강화, 연막 안에 있으면 치명이 오른다
UCLASS()
class PW_API USpecialForcesSmokeUse : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	USpecialForcesSmokeUse();

	virtual void Execute_Conditional() override;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};
