//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "TestAilment.generated.h"
//테스트용 고정 피해 상태이상
UCLASS()
class PW_API UTestAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	UTestAilment();
	virtual void Execute(ACharacterBase* affected) override;
};
