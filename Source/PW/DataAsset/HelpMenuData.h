//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset/TutorialData.h"
#include "HelpMenuData.generated.h"

//도움말 항목 하나, 왼쪽 목록의 버튼 하나에 대응한다
USTRUCT(BlueprintType)
struct FHelpTopic
{
	GENERATED_BODY()

	//목록 버튼에 표시할 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Help")
	FText title;

	//비워 두면 목록에 나오지 않는다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Help")
	TArray<FTutorialPage> pages;
};

//도움말 화면의 항목 목록, 배열 순서가 곧 표시 순서다
UCLASS(BlueprintType)
class PW_API UHelpMenuData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Help")
	TArray<FHelpTopic> topics;
};
