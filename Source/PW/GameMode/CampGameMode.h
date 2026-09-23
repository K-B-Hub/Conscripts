//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CampGameMode.generated.h"

class AAllyCharacterBase;
class ACampSpawnPoint;
class UCampOptionData;

//야영지 레벨의 게임모드
//로스터 전원을 자리에 세워두고 정비 선택 하나를 받은 뒤 다음 스테이지로 보낸다
//전투와 달리 턴도 이동도 없으며 카메라는 레벨에 배치된 CameraActor로 고정된다
UCLASS()
class PW_API ACampGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACampGameMode();

	//야영지에 서 있는 아군, 정비 효과의 적용 대상
	const TArray<TObjectPtr<AAllyCharacterBase>>& GetCampAllies() const { return campAllies; }

	//새로 합류한 동료를 자리에 세운다, 증원·보급이 사용
	//남은 자리가 없으면 nullptr
	AAllyCharacterBase* AddRecruit(TSubclassOf<AAllyCharacterBase> jobClass, const FString& displayName, int32 level);

	//현재 로스터의 평균 레벨, 충원 동료의 레벨 기준
	int32 GetAverageLevel() const;

	//정비를 마치고 야영지를 떠난다, 회수 후 다음 스테이지로
	void LeaveCamp();

	//이 야영지가 제시하는 정비 선택지
	const TArray<TObjectPtr<UCampOptionData>>& GetCampOptions() const { return campOptions; }

protected:
	virtual void BeginPlay() override;

	//회복·휴식·증원·보급, BP에서 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<TObjectPtr<UCampOptionData>> campOptions;

private:
	//자리 수집 후 로스터 전원을 세운다
	void SpawnRoster();

	//레벨에 배치된 CameraActor를 뷰 타깃으로 삼는다
	void ApplyFixedCamera();

	//비어 있는 다음 자리, 없으면 nullptr
	ACampSpawnPoint* TakeFreePoint();

	//야영지에 세운 아군
	UPROPERTY()
	TArray<TObjectPtr<AAllyCharacterBase>> campAllies;

	//레벨에 배치된 자리, 발견 순서대로 채운다
	UPROPERTY()
	TArray<TObjectPtr<ACampSpawnPoint>> spawnPoints;
};