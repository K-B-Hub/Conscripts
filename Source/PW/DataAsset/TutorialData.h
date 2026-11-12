//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TutorialData.generated.h"

class UTexture2D;

//튜토리얼 카드 한 장
USTRUCT(BlueprintType)
struct FTutorialPage
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial")
	FText title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial", meta = (MultiLine = true))
	FText body;

	//설명용 스크린샷, 비우면 글만 표시
	//소프트 참조라 장을 펼칠 때만 메모리에 올라온다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial")
	TSoftObjectPtr<UTexture2D> image;
};

//화면 하나의 튜토리얼, 그 화면에 처음 닿았을 때 한 번 뜬다
UCLASS(BlueprintType)
class PW_API UTutorialData : public UDataAsset
{
	GENERATED_BODY()

public:
	//세이브에 기록되는 식별자, 에셋을 옮기거나 이름을 바꿔도 유지되도록 별도 지정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial")
	FName tutorialId;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tutorial")
	TArray<FTutorialPage> pages;
};
