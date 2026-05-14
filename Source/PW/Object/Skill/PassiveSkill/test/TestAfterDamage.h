// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "TestAfterDamage.generated.h"

class ACharacterBase;

// 테스트용 Reactive 패시브 — AfterDamage 시점에 발동
// ReflectDamage가 실제 데미지를 적용한 직후 공격자 측에서 호출됨
UCLASS()
class PW_API UTestAfterDamage : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	UTestAfterDamage();

	virtual void Execute_AfterDamage(ACharacterBase* target) override;
};