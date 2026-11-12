//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HelpTopicButton.generated.h"

class UButton;
class UTextBlock;

//항목을 클릭하면 도움말 데이터에서의 순번을 통지
DECLARE_DELEGATE_OneParam(FOnHelpTopicClicked, int32);

//도움말 항목 하나에 대응하는 버튼, 도움말 화면이 동적으로 생성
UCLASS()
class PW_API UHelpTopicButton : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitTopic(int32 inTopicIndex, const FText& title);

	//지금 펼쳐진 항목인지 표시
	void SetSelected(bool bSelected);

	int32 GetTopicIndex() const { return topicIndex; }

	//클릭 통지, 도움말 화면이 바인딩
	FOnHelpTopicClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> TopicButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;

	//펼쳐진 항목에만 보이는 강조
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> SelectedMark;

private:
	UFUNCTION()
	void HandleClicked();

	int32 topicIndex = INDEX_NONE;
};
