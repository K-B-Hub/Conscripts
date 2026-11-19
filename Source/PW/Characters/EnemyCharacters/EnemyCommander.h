//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyBase.h"
#include "EnemyCommander.generated.h"

//적 지휘관, 스킬·강화 테이블은 BP에서 지정한다
UCLASS()
class PW_API AEnemyCommander : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyCommander();
};
