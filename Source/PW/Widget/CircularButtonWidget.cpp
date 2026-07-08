// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CircularButtonWidget.h"

#include "Components/Image.h"

void UCircularButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	//에디터 미리보기와 런타임 초기 상태 모두 노말 텍스처로 시작
	ApplyTexture(NormalTexture);
}

void UCircularButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//이미지가 히트 대상이 되어야 마우스 이벤트가 위젯으로 버블링됨
	if (ButtonImage)
	{
		ButtonImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void UCircularButtonWidget::SetIsEnabled(bool bInIsEnabled)
{
	Super::SetIsEnabled(bInIsEnabled);

	bPressed = false;
	ApplyTexture(NormalTexture);
	if (ButtonImage)
	{
		//비활성화 시 회색 틴트로 표시
		ButtonImage->SetColorAndOpacity(bInIsEnabled ? FLinearColor::White : FLinearColor(0.4f, 0.4f, 0.4f, 1.f));
	}
}

FReply UCircularButtonWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//원 밖 클릭은 Unhandled로 뒤 위젯에 통과
	if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || !IsInsideCircle(InGeometry, InMouseEvent))
	{
		return FReply::Unhandled();
	}

	bPressed = true;
	ApplyTexture(PressedTexture);
	//드래그로 벗어나도 ButtonUp을 받도록 마우스 캡처
	return FReply::Handled().CaptureMouse(TakeWidget());
}

FReply UCircularButtonWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!bPressed)
	{
		return FReply::Unhandled();
	}

	bPressed = false;
	//원 안에서 뗐을 때만 클릭 성립
	if (IsInsideCircle(InGeometry, InMouseEvent))
	{
		ApplyTexture(HoveredTexture);
		OnClicked.Broadcast();
	}
	else
	{
		ApplyTexture(NormalTexture);
	}
	return FReply::Handled().ReleaseMouseCapture();
}

FReply UCircularButtonWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	//사각 히트 영역 안에서도 원 기준으로 호버/노말 판정
	const bool bInside = IsInsideCircle(InGeometry, InMouseEvent);
	if (bPressed)
	{
		ApplyTexture(bInside ? PressedTexture : NormalTexture);
	}
	else
	{
		ApplyTexture(bInside ? HoveredTexture : NormalTexture);
	}
	return FReply::Unhandled();
}

void UCircularButtonWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	if (!bPressed)
	{
		ApplyTexture(NormalTexture);
	}
}

bool UCircularButtonWidget::IsInsideCircle(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) const
{
	const FVector2D local = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const FVector2D center = InGeometry.GetLocalSize() * 0.5f;
	const float radius = center.X;
	return FVector2D::DistSquared(local, center) <= radius * radius;
}

void UCircularButtonWidget::ApplyTexture(UTexture2D* InTexture)
{
	if (ButtonImage && InTexture)
	{
		ButtonImage->SetBrushFromTexture(InTexture);
	}
}
