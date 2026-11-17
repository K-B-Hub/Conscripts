//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ally/RifleMan/Random/Middle/Skill/RifleSmokeGrenade.h"
#include "SpecialForcesSmokeGrenade.generated.h"

//레벨 15 고정 강화, 연막탄을 던져 숨는다
//소총수 연막탄과 같은 동작, 사거리와 비용만 다르다
//지속 라운드는 지형의 lifeRound라 BP에서 1라운드 연막 지형으로 바꿔 끼운다
UCLASS()
class PW_API USpecialForcesSmokeGrenade : public URifleSmokeGrenade
{
	GENERATED_BODY()

public:
	USpecialForcesSmokeGrenade();
};
