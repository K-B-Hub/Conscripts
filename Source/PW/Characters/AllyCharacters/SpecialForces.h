//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "SpecialForces.generated.h"

//특수부대원, 근거리 암살자
//고유 자원은 장비이며 스킬·강화는 BP에서 지정한다
UCLASS()
class PW_API ASpecialForces : public AAllyCharacterBase
{
	GENERATED_BODY()

public:
	ASpecialForces();
};
