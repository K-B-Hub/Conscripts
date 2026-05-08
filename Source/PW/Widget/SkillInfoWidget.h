// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum/SkillTypes.h"
#include "Blueprint/UserWidget.h"
#include "SkillInfoWidget.generated.h"

class UTextBlock;

// 스킬 적용 대상 캐릭터 머리 위에 표시되는 전투 예측 위젯
// AttackRangeIndicator 오버랩 시 pendingDMG, pendingAccuracy, pendingCritical 표시
UCLASS()
class PW_API USkillInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// 전투 예측 값 갱신
	void UpdateInfo(float Damage, float Accuracy, float Critical, ESkillType SkillType);

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> DamageText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AccuracyText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> CriticalText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BuffText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AilmentText;
};
