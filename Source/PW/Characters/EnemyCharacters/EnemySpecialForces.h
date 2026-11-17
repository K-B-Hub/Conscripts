//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemySpecialForces.generated.h"

//적 특수부대원, 스킬·강화 테이블은 BP에서 아군 특수부대원 것을 지정한다
UCLASS()
class PW_API AEnemySpecialForces : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemySpecialForces();
};
