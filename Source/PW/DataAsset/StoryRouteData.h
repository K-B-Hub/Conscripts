//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StoryRouteData.generated.h"

//스토리 모드의 줄기 하나, 줄기 선택 화면의 항목이자 클리어 기록의 단위
//해금 직업 매핑은 편성 단계에서, 스테이지 시퀀스는 런 흐름 단계에서 추가
UCLASS(BlueprintType)
class PW_API UStoryRouteData : public UDataAsset
{
	GENERATED_BODY()

public:
	//세이브에 기록되는 식별자, 에셋을 옮기거나 이름을 바꿔도 유지되도록 별도 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	FName routeId;

	//줄기 선택 화면에 표시할 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	FText displayName;
};