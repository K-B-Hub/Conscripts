// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "TestAilment.generated.h"

/**
 * 매 턴 시작시 affected에게 방어력을 무시한 고정 5 피해
 */
UCLASS()
class PW_API UTestAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	UTestAilment();

	virtual void Execute(ACharacterBase* affected) override;
};