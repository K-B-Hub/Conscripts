//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ally/Sniper/Random/Middle/Ailment/SniperBleedAilment.h"
#include "SniperOpenWoundAilment.generated.h"

//상처 헤집기가 부여하는 상태이상, 출혈과 같은 피해를 더 짧게 준다
//출혈탄과 클래스가 달라 둘 다 걸리면 각각 피해를 준다
UCLASS()
class PW_API USniperOpenWoundAilment : public USniperBleedAilment
{
	GENERATED_BODY()

public:
	USniperOpenWoundAilment();
};
