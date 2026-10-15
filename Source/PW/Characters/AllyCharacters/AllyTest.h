// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "AllyTest.generated.h"

//테스트용 아군 캐릭터, 스킬은 BP의 defaultSkills에서 지정한다
UCLASS()
class PW_API AAllyTest : public AAllyCharacterBase
{
	GENERATED_BODY()

public:
	AAllyTest();

protected:
	virtual void BeginPlay() override;
};
