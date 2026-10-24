//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampOptionButton.generated.h"

class UButton;
class UTextBlock;
class UImage;
class UCampOptionData;

//항목 클릭 시 목록 내 인덱스를 통지
DECLARE_DELEGATE_OneParam(FOnCampOptionClicked, int32);

//야영지 정비 선택지 카드 하나, 선택지 데이터와 인덱스를 받는다
//캐릭터 후보 목록과 달리 고르는 대상이 캐릭터가 아니라 선택지라 UJobSelectButton과 나뉘어 있다
UCLASS()
class PW_API UCampOptionButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 내용과 통지할 인덱스 설정
	void InitEntry(const UCampOptionData* option, int32 index);

	//클릭 통지, 목록을 만든 쪽이 바인딩
	FOnCampOptionClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JobButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DescText;

private:
	UFUNCTION()
	void HandleClicked();

	//통지할 목록 인덱스
	int32 entryIndex = INDEX_NONE;
};
