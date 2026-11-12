//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widget/TutorialWidget.h"
#include "HelpWidget.generated.h"

class UHelpMenuData;
class UHelpTopicButton;
class UPanelWidget;

//도움말 화면, 왼쪽 목록에서 고른 항목의 장들을 오른쪽에 보여준다
//장 넘김과 바인딩은 튜토리얼 카드의 것을 그대로 쓴다
UCLASS()
class PW_API UHelpWidget : public UTutorialWidget
{
	GENERATED_BODY()

public:
	//닫기 통지, 복귀 방법은 여는 쪽이 정한다
	//허브는 화면 교체이고 전투 일시정지는 겹침 제거라 이쪽이 알 수 없다
	FSimpleDelegate onClosed;

	//본 기록을 건드리지 않도록 GameInstance에 알리지 않는다
	virtual void Close() override;

protected:
	virtual void NativeConstruct() override;

	//표시할 항목들, WBP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "Help")
	TObjectPtr<UHelpMenuData> helpData;

	//항목 버튼 위젯 클래스, WBP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UHelpTopicButton> topicButtonClass;

	//항목 버튼이 붙을 컨테이너
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> TopicList;

private:
	//항목 목록으로 버튼을 만들어 컨테이너에 채우고 첫 항목을 펼친다
	void BuildTopicList();

	void SelectTopic(int32 topicIndex);

	UPROPERTY()
	TArray<TObjectPtr<UHelpTopicButton>> topicButtons;
};
