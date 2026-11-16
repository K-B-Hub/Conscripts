//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CameraBoundsVolume.generated.h"

class UBoxComponent;

//전투 카메라 자유 이동 구역, 여러 개면 전부 감싼 범위가 경계가 된다
//판정은 월드 축 정렬 박스라 회전시키면 회전된 박스를 감싼 더 큰 범위로 잡힌다
UCLASS()
class PW_API ACameraBoundsVolume : public AActor
{
	GENERATED_BODY()

public:
	ACameraBoundsVolume();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	TObjectPtr<UBoxComponent> boundsBox;
};
