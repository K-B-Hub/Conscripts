//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "HighFirstAidPassive.generated.h"

//공용 상급 강화, 턴이 시작할 때 체력을 10% 회복한다
UCLASS()
class PW_API UHighFirstAidPassive : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UHighFirstAidPassive();

	virtual void Execute_Conditional() override;

protected:
	//턴 시작 회복 비율, 0.1 = 최대 체력의 10%
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PassiveType")
	float healRatio = 0.1f;
};
