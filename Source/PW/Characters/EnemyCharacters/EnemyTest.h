// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemyTest.generated.h"

//테스트용 적 캐릭터
UCLASS()
class PW_API AEnemyTest : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyTest();

protected:
	//테스트 액티브 스킬 장착
	virtual void SetDefaultSkills() override;
	//virtual void SetDefaultPassives() override;
};
