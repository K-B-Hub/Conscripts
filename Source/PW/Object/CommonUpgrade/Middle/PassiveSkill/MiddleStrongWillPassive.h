//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "MiddleStrongWillPassive.generated.h"

//공용 중급 강화, 체력이 40% 이하일 때 회피와 명중이 25 증가한다
UCLASS()
class PW_API UMiddleStrongWillPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UMiddleStrongWillPassive();

	virtual void Execute_Conditional() override;

protected:
	//발동 체력 비율, 0.4 = 최대 체력의 40% 이하
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	float hpThreshold = 0.4f;

private:
	//현재 스탯이 적용된 상태인지, 중복 가감 방지용
	UPROPERTY()
	bool bApplied = false;
};