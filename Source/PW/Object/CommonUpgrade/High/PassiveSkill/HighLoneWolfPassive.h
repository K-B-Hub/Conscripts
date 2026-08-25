//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighLoneWolfPassive.generated.h"

//공용 상급 강화, 주위 5m 내에 다른 아군이 없을 때 공격력·속도·기술·방어력이 5씩 증가한다
UCLASS()
class PW_API UHighLoneWolfPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighLoneWolfPassive();

	virtual void Execute_Conditional() override;

protected:
	//고립 판정 거리, 500 = 5m
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	float isolationRange = 500.f;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};