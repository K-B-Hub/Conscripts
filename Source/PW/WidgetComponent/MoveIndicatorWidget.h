// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "MoveIndicatorWidget.generated.h"

/**
 * 커서 인디케이터에 부착되는 이동 거리 표시 위젯 컴포넌트.
 * WBP에서 PathDistanceMeters를 Property Binding으로 읽어 거리 텍스트를 표시한다.
 */
UCLASS()
class PW_API UMoveIndicatorWidget : public UWidgetComponent
{
	GENERATED_BODY()

public:
	/** WBP에서 Property Binding으로 읽어가는 경로 거리 (m) */
	UPROPERTY(BlueprintReadOnly, Category = "Move")
	float PathDistanceMeters = 0.f;

	/** CursorIndicator에서 매 프레임(쓰로틀링) 호출 */
	void UpdateDistance(float Meters);
};
