//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "RunProgress.generated.h"

//한 번의 런 동안 유지되는 진행 상태
//레벨 전환에도 살아남아야 하므로 GameInstance가 소유하고, 런 시작 시 통째로 새로 만든다
//로스터와 스테이지 시퀀스는 이후 단계에서 추가
UCLASS()
class PW_API URunProgress : public UObject
{
	GENERATED_BODY()

public:
	//편성 확정 시 호출, 런을 연다. 스토리가 아니면 routeId는 NAME_None
	void StartRun(FName inStoryRouteId);

	//런 종료, 메인메뉴 복귀 시 호출
	void EndRun();

	//현재 스테이지 번호, 0부터 시작
	int32 GetStageIndex() const { return stageIndex; }

	bool IsRunActive() const { return bRunActive; }

	//스토리 모드에서 진행 중인 줄기, 다른 모드에서는 NAME_None
	//런이 열릴 때 확정되므로 별도 setter를 두지 않는다
	FName GetStoryRouteId() const { return storyRouteId; }

private:
	//진행 중인 스테이지 번호
	int32 stageIndex = 0;

	//런 진행 여부
	bool bRunActive = false;

	//스토리 줄기 식별자, 클리어 시 세이브에 기록할 대상
	FName storyRouteId = NAME_None;
};