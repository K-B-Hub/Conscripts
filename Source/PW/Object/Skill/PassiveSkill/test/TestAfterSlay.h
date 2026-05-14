// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestAfterSlay.generated.h"

// 테스트용 Reactive 패시브 — AfterSlay 시점에 발동
// 공격자(=owner)가 대상을 처치한 직후 호출, slainLocation은 죽은 캐릭터의 위치
UCLASS()
class PW_API UTestAfterSlay : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestAfterSlay();

	virtual void Execute_AfterSlay(FVector slainLocation) override;
};