// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/UpgradeChoiceButton.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Object/Skill/SkillBase.h"

void UUpgradeChoiceButton::NativeConstruct()
{
	Super::NativeConstruct();

	SetIsFocusable(false);
	
	if (ChoiceButton)
	{
		ChoiceButton->OnClicked.AddDynamic(this, &UUpgradeChoiceButton::HandleClicked);
	}
}

void UUpgradeChoiceButton::InitChoice(TSubclassOf<USkillBase> InSkillClass)
{
	skillClass = InSkillClass;
	if (!skillClass) return;

	//습득 전이라 인스턴스가 없으므로 CDO에서 표시 정보 조회
	const USkillBase* cdo = skillClass->GetDefaultObject<USkillBase>();
	if (NameText)
	{
		NameText->SetText(cdo->skillName);
		//텍스트가 버튼 위를 덮어 클릭을 가로채지 않도록 hit-test 제외
		NameText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (DescText)
	{
		DescText->SetText(cdo->skillDescription);
		DescText->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	if (SkillIcon)
	{
		//ImageSize는 WBP에 잡아둔 값을 유지한다, 아이콘 해상도가 카드 크기를 흔들지 않게
		if (cdo->skillIcon)
		{
			SkillIcon->SetBrushFromTexture(cdo->skillIcon);
			SkillIcon->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			//Hidden으로 자리를 남긴다, Collapsed면 이름·설명이 위로 밀려 카드마다 배치가 달라진다
			SkillIcon->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UUpgradeChoiceButton::HandleClicked()
{
	UE_LOG(LogTemp, Log, TEXT("[Upgrade] 버튼 클릭 도달: %s"), skillClass ? *skillClass->GetName() : TEXT("null"));
	OnClicked.ExecuteIfBound(skillClass);
}
