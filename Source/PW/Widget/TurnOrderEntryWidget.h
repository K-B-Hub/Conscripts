// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Widget/ImageButton.h"
#include "TurnOrderEntryWidget.generated.h"

class UImage;
class ACharacterBase;

//턴 순서 칸 하나, 캐릭터 초상화와 현재 턴 여부 표시
//목록에는 이번 라운드에 아직 행동하지 않은 칸만 남으므로 상태는 현재 턴이냐 아니냐 둘뿐이다
UCLASS()
class PW_API UTurnOrderEntryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//대응 캐릭터 설정
	void InitEntry(ACharacterBase* InCharacter);

	//현재 턴 칸인지에 따라 시각 처리 갱신
	void SetCurrent(bool bIsCurrent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	//초상화 호버는 월드 호버와 같은 미리보기를 띄우고 월드의 대응 캐릭터를 강조한다
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	//칸이 가린 자리의 클릭을 삼킨다, 버튼이 받은 클릭은 여기까지 올라오지 않는다
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	//BP에서 "PortraitImage" 이름으로 바인딩, 버튼 위에 겹치므로 HitTestInvisible로 둔다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> PortraitImage;

	//현재 턴 마커 (예: NOW 텍스트), BP에 있으면 자동 바인딩
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> NowMarker;

	//칸의 클릭 대상이자 진영·턴 상태 아트를 그리는 버튼
	//클릭은 카메라를 그 캐릭터로 옮긴다, 상세창을 여는 입구는 만들지 않는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImageButton> TeamFrame;

	//진영 × 현재 턴 여부의 네 조합이 각자 기본/호버/클릭 아트를 쓴다
	UPROPERTY(EditDefaultsOnly, Category = "TurnOrder|Art")
	FImageButtonArt allyCurrentArt;

	UPROPERTY(EditDefaultsOnly, Category = "TurnOrder|Art")
	FImageButtonArt allyIdleArt;

	UPROPERTY(EditDefaultsOnly, Category = "TurnOrder|Art")
	FImageButtonArt enemyCurrentArt;

	UPROPERTY(EditDefaultsOnly, Category = "TurnOrder|Art")
	FImageButtonArt enemyIdleArt;

private:
	UFUNCTION()
	void HandleFrameClicked();

	//캐릭터 진영과 현재 턴 여부로 네 세트 중 하나를 골라 버튼에 꽂는다
	void ApplyFrameArt();

	UPROPERTY()
	TObjectPtr<ACharacterBase> character = nullptr;

	//아트 선택에 쓰이므로 SetCurrent가 준 값을 들고 있어야 한다
	bool bCurrentTurn = false;
};
