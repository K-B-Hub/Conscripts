//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CampDialogueData.generated.h"

class AAllyCharacterBase;

//야영지에서 아군에게 마우스를 올렸을 때 나오는 대사 풀
//직업이 아니라 상태에 따라 갈리므로 직업 BP가 아닌 공용 DataAsset에 둔다
UCLASS(BlueprintType)
class PW_API UCampDialogueData : public UDataAsset
{
	GENERATED_BODY()

public:
	//체력이 부상 기준 아래일 때
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<FText> woundedLines;

	//스트레스가 기준 이상일 때
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<FText> stressedLines;

	//별다른 이상이 없을 때
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	TArray<FText> healthyLines;

	//부상으로 판정할 체력 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	float woundedHpRatio = 0.5f;

	//고스트레스로 판정할 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	float stressedRatio = 0.5f;

	//캐릭터 상태에 맞는 대사 하나를 고른다, 해당 풀이 비었으면 건강 풀로 내려간다
	FText PickLine(const AAllyCharacterBase* ally) const;
};