// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "TestBuff.generated.h"

/**
 * 
 */
UCLASS()
class PW_API UTestBuff : public UBuffBase
{
	GENERATED_BODY()
	
public:
	UTestBuff();	

	virtual void Execute(ACharacterBase* affected, ACharacterBase* caster) override;
};
