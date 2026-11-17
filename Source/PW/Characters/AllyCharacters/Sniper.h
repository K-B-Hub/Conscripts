//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "Sniper.generated.h"

//저격수, 초원거리 딜러
//고유 자원은 탄환이며 스킬·강화는 BP에서 지정한다
UCLASS()
class PW_API ASniper : public AAllyCharacterBase
{
	GENERATED_BODY()

public:
	ASniper();
};
