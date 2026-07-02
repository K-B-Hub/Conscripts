// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "AllyTest.generated.h"

//테스트용 아군 캐릭터, 모든 테스트 스킬/패시브를 장착
UCLASS()
class PW_API AAllyTest : public AAllyCharacterBase
{
	GENERATED_BODY()

public:
	AAllyTest();

protected:
	virtual void BeginPlay() override;

	//테스트 액티브 스킬 장착
	virtual void SetDefaultSkills() override;
	//테스트 패시브 스킬 장착
	virtual void SetDefaultPassives() override;
};
