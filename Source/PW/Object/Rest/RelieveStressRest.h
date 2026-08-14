//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Rest/RestBase.h"
#include "RelieveStressRest.generated.h"

//1회성 즉시 효과, 팀 전원의 스트레스를 최대치 비율만큼 감소
UCLASS()
class PW_API URelieveStressRest : public URestBase
{
	GENERATED_BODY()

public:
	URelieveStressRest();
	virtual void Execute(ACharacterBase* instigator) override;

protected:
	//최대 스트레스 대비 감소 비율, 0.3 = 30%
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Rest")
	float relieveRatio = 0.3f;
};