// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestBeforeCalc.generated.h"

class ACharacterBase;

// 테스트용 Reactive 패시브 — BeforeDamageCalc 시점에 발동 조건 평가
// 조건 충족시 true 반환 → 호출자가 본 인스턴스의 보너스 필드를 합산
UCLASS()
class PW_API UTestBeforeCalc : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestBeforeCalc();

	virtual bool Execute_BeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType) override;
};