//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SquadSpawnPoint.generated.h"

//적 한 분대가 들어설 자리, 전투 맵에 정원보다 넉넉히 뿌려두고 일부만 추첨한다
//대형은 USquadData가 들고 있으므로 이 액터는 위치와 방향만 정한다
//엄폐물을 등지고 통로를 보게 회전시켜 두는 것이 이 액터의 전부이며, 자연스러움은 거기서 나온다
UCLASS()
class PW_API ASquadSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ASquadSpawnPoint();
};