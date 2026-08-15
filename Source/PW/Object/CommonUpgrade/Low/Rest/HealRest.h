//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Rest/RestBase.h"
#include "HealRest.generated.h"

//1회성 즉시 효과, 팀 전원의 체력을 최대 체력 비율만큼 회복
UCLASS()
class PW_API UHealRest : public URestBase
{
	GENERATED_BODY()

public:
	UHealRest();
	virtual void Execute(ACharacterBase* instigator) override;

protected:
	//최대 체력 대비 회복 비율, 0.3 = 30%
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest")
	float healRatio = 0.3f;
};