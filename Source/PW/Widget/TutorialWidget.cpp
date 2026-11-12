//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/TutorialWidget.h"
#include "GameInstance/PWGameInstance.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PrevButton)  PrevButton->OnClicked.AddDynamic(this, &UTutorialWidget::HandlePrevClicked);
	if (NextButton)  NextButton->OnClicked.AddDynamic(this, &UTutorialWidget::HandleNextClicked);
	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &UTutorialWidget::Close);
}

FReply UTutorialWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

FReply UTutorialWidget::NativeOnMouseButtonDoubleClick(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//더블클릭은 Down과 다른 경로로 오고, 액터 클릭 이벤트는 이것도 클릭으로 친다
	return FReply::Handled();
}

void UTutorialWidget::Open(const UTutorialData* inData)
{
	data = inData;
	bReachedLastPage = false;

	if (data) SetPages(data->pages);
}

void UTutorialWidget::SetPages(const TArray<FTutorialPage>& inPages)
{
	pages = inPages;
	ShowPage(0);
}

void UTutorialWidget::Close()
{
	RemoveFromParent();

	if (UPWGameInstance* gameInstance = GetGameInstance<UPWGameInstance>())
	{
		gameInstance->NotifyTutorialClosed(data, bReachedLastPage);
	}
}

void UTutorialWidget::HandlePrevClicked()
{
	ShowPage(pageIndex - 1);
}

void UTutorialWidget::HandleNextClicked()
{
	if (pageIndex >= pages.Num() - 1)
	{
		if (bCloseOnLastPage) Close();
		return;
	}

	ShowPage(pageIndex + 1);
}

void UTutorialWidget::ShowPage(int32 index)
{
	if (!pages.IsValidIndex(index)) return;

	pageIndex = index;

	const FTutorialPage& page = pages[index];
	const int32 pageCount = pages.Num();
	const bool bLastPage = index == pageCount - 1;
	if (bLastPage) bReachedLastPage = true;

	if (TitleText) TitleText->SetText(page.title);
	BodyText->SetText(page.body);

	//원본 크기로 맞춰야 바깥 ScaleBox가 비율을 지킨다
	UTexture2D* texture = page.image.LoadSynchronous();
	if (texture) PageImage->SetBrushFromTexture(texture, true);
	ImagePanel->SetVisibility(texture ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);

	//라벨이 버튼의 형제라 숨기면 글자만 남는다, 비활성으로 둔다
	PrevButton->SetIsEnabled(index > 0);
	NextButton->SetIsEnabled(bCloseOnLastPage || !bLastPage);

	if (NextText)
	{
		NextText->SetText(bLastPage ? NSLOCTEXT("Tutorial", "Done", "확인") : NSLOCTEXT("Tutorial", "Next", "다음"));
	}

	if (PageText)
	{
		PageText->SetText(FText::Format(INVTEXT("{0} / {1}"), FText::AsNumber(index + 1), FText::AsNumber(pageCount)));
	}
}
