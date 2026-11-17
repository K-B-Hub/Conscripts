//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "SniperConfusionDebuff.generated.h"

//혼란 유발이 부여하는 디버프, 이동할 수 없고 명중과 회피가 크게 줄어든다
//이동력 감소량은 적용 시점 대상의 이동력을 스냅샷한다
UCLASS()
class PW_API USniperConfusionDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	USniperConfusionDebuff();

	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster) override;
};
