// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "AllyTest.generated.h"

/**
 * 
 */
UCLASS()
class PW_API AAllyTest : public AAllyCharacterBase
{
	GENERATED_BODY()
	
public:
	AAllyTest();
	
protected:
	virtual void BeginPlay() override;
	
private:
	virtual void SetDefaultSkills() override;
};
