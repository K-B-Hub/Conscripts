//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "RifleMan.generated.h"

//소총수, 중거리 전투원
//고유 자원은 탄창이며 스킬·강화는 BP에서 지정한다
UCLASS()
class PW_API ARifleMan : public AAllyCharacterBase
{
	GENERATED_BODY()

public:
	ARifleMan();
};
