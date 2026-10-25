//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StatusEffectIcon.generated.h"

class UImage;
class UPanelWidget;
class UTexture2D;
class ACharacterBase;
class USkillTooltipWidget;

//버프·상태이상·스탯 가감 패시브 공통 표시 정보, 공통 부모가 없어 UI용으로만 모은다
struct FStatusEffectView
{
	UTexture2D* icon = nullptr;
	FText name;
	FText description;
	//턴 제한이 없는 패시브는 INDEX_NONE
	int32 remainingTurn = INDEX_NONE;
};

//걸려 있는 버프·상태이상 하나의 아이콘, 툴팁 클래스가 지정되면 호버 시 툴팁이 뜬다
UCLASS()
class PW_API UStatusEffectIcon : public UUserWidget
{
	GENERATED_BODY()

public:
	//대상의 버프·패시브 효과·상태이상 순으로 컨테이너를 다시 채운다
	static void FillContainer(UPanelWidget* container, TSubclassOf<UStatusEffectIcon> iconClass,
		const ACharacterBase* target, UUserWidget* owner);

	void InitEntry(const FStatusEffectView& view);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> EffectIcon;

	//스킬 툴팁을 그대로 쓴다, 비우면 툴팁 없이 아이콘만 보인다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillTooltipWidget> tooltipClass;
};
