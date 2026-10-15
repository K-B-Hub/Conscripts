//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MovingPointWidget.generated.h"

class ACharacterBase;
class UProgressBar;
class UTextBlock;

//현재 턴 유닛의 이동력 표시, 연속값이라 원형 게이지가 아니라 프로그레스 바를 쓴다
//행동력·전투 자원과 달리 이동 중 프레임마다 깎이므로 델리게이트가 아니라 폴링으로 읽는다
UCLASS()
class PW_API UMovingPointWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//턴 시작 시 추적 대상 지정, 이후 NativeTick이 이 유닛을 폴링한다
	void InitMovingPoint(ACharacterBase* unit);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> MovingPointBar;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ValueText;

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> trackedUnit = nullptr;

	//마지막으로 표시한 값, FText 생성이 매 프레임 할당이라 표시되는 숫자가 변할 때만 쓴다
	//소수 첫째 자리까지 보여주므로 10배 정수가 표시 단위다
	int32 shownTenths = -1;
};