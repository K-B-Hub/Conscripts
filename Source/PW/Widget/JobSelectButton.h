//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JobSelectButton.generated.h"

class UButton;
class UTextBlock;

//항목 클릭 시 목록 내 인덱스를 통지
DECLARE_DELEGATE_OneParam(FOnJobButtonClicked, int32);

//편성 화면의 목록 항목 하나, 직업 후보와 편성된 인원 양쪽에 쓰인다
//표시할 문구와 인덱스만 받으므로 어느 목록에 속하는지는 상위 위젯이 안다
UCLASS()
class PW_API UJobSelectButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 문구와 통지할 인덱스 설정
	void InitEntry(const FText& label, int32 index);

	//클릭 통지, 편성 위젯이 바인딩
	FOnJobButtonClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JobButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LabelText;

private:
	UFUNCTION()
	void HandleClicked();

	//통지할 목록 인덱스
	int32 entryIndex = INDEX_NONE;
};