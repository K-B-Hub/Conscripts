//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "RifleTearGasAilment.generated.h"

//최루탄이 부여하는 상태이상, 턴 시작마다 최대 체력 비율 피해를 준다
UCLASS()
class PW_API URifleTearGasAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	URifleTearGasAilment();

	virtual void Execute(ACharacterBase* affected) override;

protected:
	//턴당 최대 체력 피해 비율(%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	int32 maxHpRatioPercent = 10;
};