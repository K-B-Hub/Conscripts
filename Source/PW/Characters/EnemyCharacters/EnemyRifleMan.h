//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemyRifleMan.generated.h"

//적 소총수, 스킬·강화 테이블은 BP에서 아군 소총수 것을 지정한다
UCLASS()
class PW_API AEnemyRifleMan : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyRifleMan();
};
