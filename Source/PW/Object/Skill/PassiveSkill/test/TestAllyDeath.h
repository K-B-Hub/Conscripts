// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestAllyDeath.generated.h"

// 테스트용 Conditional 패시브 — AllyDeath (전역 이벤트, 아군 사망)
UCLASS()
class PW_API UTestAllyDeath : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestAllyDeath();

	virtual void Execute_Conditional() override;
};