//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Run/StageEntry.h"
#include "Widget/ImageButton.h"
#include "StoryRouteData.generated.h"

class AAllyCharacterBase;

//스토리 모드의 줄기 하나, 줄기 선택 화면의 항목이자 클리어 기록의 단위
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

	//줄기 항목 버튼의 상태별 아트, 한 위젯 클래스를 줄기마다 생성하므로 그림은 데이터가 들고 있다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	FImageButtonArt buttonArt;

	//이 줄기에서 고를 수 있는 캐릭터, 편성과 증원이 모두 이 목록에서만 뽑는다
	//스토리는 해금과 무관하게 줄기가 정한 인물들로만 싸운다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	TArray<TSubclassOf<AAllyCharacterBase>> selectableCharacters;

	//이 줄기가 진행할 스테이지 순서, 줄기가 곧 시나리오이므로 시퀀스도 줄기가 소유한다
	//비워 두면 GameInstance의 기본 시퀀스를 쓴다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	TArray<FStageEntry> stages;

	//이 줄기를 클리어하면 다른 모드에서 쓸 수 있게 되는 캐릭터
	//세이브에는 줄기만 남으므로 이 매핑을 고치면 기존 세이브에도 그대로 반영된다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Story")
	TArray<TSubclassOf<AAllyCharacterBase>> unlockedJobs;
};