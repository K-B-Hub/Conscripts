//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MissionData.generated.h"

class ABattleGameMode;

//스테이지의 승리 조건
//타입마다 필요한 데이터가 달라(암살=대상, 도달=위치, 생존=턴 수) 파생 클래스로 나눈다
//변형은 파생 클래스를 또 만들지 않고 DataAsset 인스턴스를 늘려서 표현한다
UCLASS(Abstract, BlueprintType)
class PW_API UMissionData : public UDataAsset
{
	GENERATED_BODY()

public:
	//출격 화면과 전투 HUD에 표시할 목표 설명
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mission")
	FText Description;

	//목표 달성 여부, 파생 클래스가 정의
	//GameMode를 통째로 받으므로 새 목표가 필요한 정보는 접근자만 추가하면 된다
	virtual bool IsComplete(const ABattleGameMode* battle) const { return false; }
};