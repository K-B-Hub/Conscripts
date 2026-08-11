//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "StunAilment.generated.h"

//스트레스 부정 이벤트 '실신', 1턴간 행동 불가
UCLASS()
class PW_API UStunAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	UStunAilment();
	virtual void Execute(ACharacterBase* affected) override;
};
