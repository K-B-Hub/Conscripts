//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "SuicidalUrgeAilment.generated.h"

//스트레스 부정 이벤트 '자살충동', 2턴간 턴 시작 시 고정 피해
UCLASS()
class PW_API USuicidalUrgeAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	USuicidalUrgeAilment();
	virtual void Execute(ACharacterBase* affected) override;
};