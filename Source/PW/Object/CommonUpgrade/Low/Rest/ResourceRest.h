//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Rest/RestBase.h"
#include "ResourceRest.generated.h"

//1회성 즉시 효과, 팀 전원의 전투 자원을 최대치 비율만큼 회복
UCLASS()
class PW_API UResourceRest : public URestBase
{
	GENERATED_BODY()

public:
	UResourceRest();
	virtual void Execute(ACharacterBase* instigator) override;

protected:
	//최대 전투 자원 대비 회복 비율, 0.3 = 30%
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest")
	float resourceRatio = 0.3f;
};