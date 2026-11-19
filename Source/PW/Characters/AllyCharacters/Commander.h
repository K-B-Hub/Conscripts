//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/AllyCharacterBase.h"
#include "Commander.generated.h"

//지휘관, 아군 지원과 적 약화 담당
//무기와 재장전은 소총수 것을 공유하고 스킬·강화는 BP에서 지정한다
UCLASS()
class PW_API ACommander : public AAllyCharacterBase
{
	GENERATED_BODY()

public:
	ACommander();
};
