//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleVeteranInstinct.generated.h"

//레벨 50 고정 강화, 명중·치명이 상시 오르고 턴 시작마다 탄창을 회복한다
UCLASS()
class PW_API URifleVeteranInstinct : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleVeteranInstinct();

	virtual void Execute_Conditional() override;

protected:
	//턴 시작마다 회복할 탄창 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Veteran")
	int32 resourceGain = 1;

private:
	//상시 스탯을 한 번만 적용하기 위한 래치, Conditional은 등록 시 스탯이 자동 적용되지 않는다
	bool bStatApplied = false;
};
