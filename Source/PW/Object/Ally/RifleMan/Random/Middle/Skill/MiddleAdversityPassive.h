//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleAdversityPassive.generated.h"

//공용 중급 강화, 지형에 있을 때 공격력이 3, 명중이 15, 치명타가 10 증가한다
UCLASS()
class PW_API UMiddleAdversityPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleAdversityPassive();

	virtual void Execute_Conditional() override;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};