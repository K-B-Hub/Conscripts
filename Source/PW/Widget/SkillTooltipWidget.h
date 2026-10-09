//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillTooltipWidget.generated.h"

class UImage;
class UTextBlock;
class USkillBase;

//스킬 아이콘·이름·효과를 보여주는 툴팁, 액티브와 패시브가 같은 위젯을 쓴다
//호출자를 모른다 — 띄울 위치는 이 위젯을 배치한 쪽의 레이아웃이 결정한다
UCLASS()
class PW_API USkillTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 내용을 채우고 보이게 한다, nullptr이면 숨긴다
	void InitTooltip(const USkillBase* skill);

	//툴팁을 거둔다
	void HideTooltip();

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillDescriptionText;

	//소모량, 액티브만 값을 가진다 — 패시브에서는 둘 다 숨는다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ActionPointCostText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> BattleResourceCostText;
};