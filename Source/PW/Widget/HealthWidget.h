// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class PW_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//최대/현재 체력을 받아 바와 수치를 갱신, 체력이 바뀌는 모든 경로가 이것만 호출한다
	void InitHealth(int32 InMaxHp, int32 InCurrentHp);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	//"11 / 20" 체력 수치, 바 위에 겹쳐 둔다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HpText;

	int32 maxHp = 1;
	int32 currentHp = 1;

	void UpdateHealthBar();
};