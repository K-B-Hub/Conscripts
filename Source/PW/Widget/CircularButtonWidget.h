// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CircularButtonWidget.generated.h"

class UImage;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCircularButtonClicked);

//원형 히트 판정 버튼, 중심 기준 반지름 내 클릭만 처리하고 밖은 뒤 위젯으로 통과
//노말/호버/클릭 텍스처를 마우스 상태에 따라 전환
UCLASS()
class PW_API UCircularButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//원 안에서 눌렀다 뗐을 때 발동
	UPROPERTY(BlueprintAssignable)
	FOnCircularButtonClicked OnClicked;

	//비활성화 시 입력 차단 + 회색 틴트
	virtual void SetIsEnabled(bool bInIsEnabled) override;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	//BP에서 "ButtonImage" 이름으로 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> ButtonImage;

	//상태별 버튼 텍스처, 파생 BP 기본값 또는 배치 인스턴스에서 지정
	UPROPERTY(EditAnywhere, Category = "CircularButton")
	TObjectPtr<UTexture2D> NormalTexture;

	UPROPERTY(EditAnywhere, Category = "CircularButton")
	TObjectPtr<UTexture2D> HoveredTexture;

	UPROPERTY(EditAnywhere, Category = "CircularButton")
	TObjectPtr<UTexture2D> PressedTexture;

private:
	bool bPressed = false;

	//로컬 좌표 기준 원 내부 여부, 위젯이 정사각 배치라고 가정
	bool IsInsideCircle(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) const;

	//텍스처 교체, null이면 유지
	void ApplyTexture(UTexture2D* InTexture);
};
