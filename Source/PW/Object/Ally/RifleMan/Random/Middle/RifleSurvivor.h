//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleSurvivor.generated.h"

//중급 랜덤 강화, 전투가 길어지면 명중과 치명이 오른다
//한 번 조건을 충족하면 그 전투 동안 풀리지 않는다
UCLASS()
class PW_API URifleSurvivor : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleSurvivor();

	virtual void Execute_Conditional() override;

protected:
	//효과가 켜지는 라운드
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	int32 startRound = 8;

private:
	//스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};