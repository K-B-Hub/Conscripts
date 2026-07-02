// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthWidget.generated.h"

class UProgressBar;

UCLASS()
class PW_API UHealthWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//캐릭터 BeginPlay에서 최대/현재 체력으로 초기화
	void InitHealth(int32 InMaxHp, int32 InCurrentHp);

	//데미지 적용 후 체력 바 갱신, 양수는 데미지 음수는 회복
	void ApplyDamage(int32 Amount);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	int32 maxHp = 1;
	int32 currentHp = 1;

	void UpdateHealthBar();
};