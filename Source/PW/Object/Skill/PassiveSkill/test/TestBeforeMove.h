// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestBeforeMove.generated.h"

// 테스트용 Reactive 패시브 — BeforeMove (이동 보너스 예시)
// 이동 시 본 패시브의 보너스 필드를 owner 스탯에 합산, 미이동 시 제거
UCLASS()
class PW_API UTestBeforeMove : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestBeforeMove();

	virtual void Execute_BeforeMove(bool bIsMoved, int32 sign) override;
};