// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemyTest.generated.h"

/**
 * 
 */
UCLASS()
class PW_API AEnemyTest : public AEnemyBase
{
	GENERATED_BODY()
	
public:
	AEnemyTest();
	
protected:	
	virtual void SetDefaultSkills() override;
	//virtual void SetDefaultPassives() override;
};
