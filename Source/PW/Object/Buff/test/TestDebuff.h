// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "TestDebuff.generated.h"

/**
 * 
 */
UCLASS()
class PW_API UTestDebuff : public UBuffBase
{
	GENERATED_BODY()
	
public:
	UTestDebuff();	

	virtual void Execute(ACharacterBase* affected) override;
};
