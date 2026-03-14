// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MoveIndicatorWidget.generated.h"

class UTextBlock;

/**
 * 이동 거리를 표시하는 UMG 위젯.
 * WBP_MoveIndicator가 이 클래스를 부모로 삼아 UI를 구성한다.
 * WBP 내 Text 위젯의 이름을 DistanceText로 맞춰야 BindWidget이 연결된다.
 */
UCLASS()
class PW_API UMoveIndicatorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** WBP의 DistanceText 위젯과 자동 연결 */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DistanceText;

	/** CursorIndicator에서 매 프레임(쓰로틀링) 호출 */
	void UpdateDistance(float Meters);
};