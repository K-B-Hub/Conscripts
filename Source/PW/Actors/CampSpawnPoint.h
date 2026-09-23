//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampSpawnPoint.generated.h"

class UBoxComponent;
class AAllyCharacterBase;

//야영지에서 아군이 설 자리, 로스터 정원 이상으로 맵에 배치한다
//캐릭터는 전투와 공유하는 클래스라 야영지 전용 충돌을 붙이지 않고, 이 자리가 마우스 감지를 대신한다
//대기 애니메이션도 스켈레톤에 묶여 있어 자리가 아니라 캐릭터가 들고 있는다
UCLASS()
class PW_API ACampSpawnPoint : public AActor
{
	GENERATED_BODY()

public:
	ACampSpawnPoint();

	//이 자리에 세운 아군, GameMode가 스폰 직후 지정
	void SetOccupant(AAllyCharacterBase* ally) { occupant = ally; }
	AAllyCharacterBase* GetOccupant() const { return occupant; }

protected:
	//마우스 감지 전용, 커서 트레이스 채널에만 반응한다
	UPROPERTY(VisibleAnywhere, Category = "Camp")
	TObjectPtr<UBoxComponent> hoverBox;

private:
	UPROPERTY()
	TObjectPtr<AAllyCharacterBase> occupant = nullptr;
};