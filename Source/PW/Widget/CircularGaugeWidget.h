// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CircularGaugeWidget.generated.h"

class UImage;
class UTextBlock;

//원형 게이지 위젯, PNG 다이얼 위에 머티리얼 원형 바와 수치 텍스트를 겹쳐 표시, HP/스트레스 공용
UCLASS()
class PW_API UCircularGaugeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//최대/현재 값으로 초기화
	void InitGauge(int32 InMax, int32 InCurrent);

	//현재 값 갱신, 바와 텍스트 동시 반영
	void SetValue(int32 InCurrent);

protected:
	//원형 바 머티리얼이 적용된 이미지, 머티리얼의 "Percent" 스칼라 파라미터로 채움 제어
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GaugeImage;

	//현재 수치 텍스트
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText;

	//최대치 텍스트 (예: "/ 60"), 스트레스처럼 불필요하면 BP에서 생략
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> MaxValueText;

private:
	int32 maxValue = 1;
	int32 currentValue = 0;

	void UpdateGauge();
};
