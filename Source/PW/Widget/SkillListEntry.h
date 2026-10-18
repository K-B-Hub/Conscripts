//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Enum/UpgradeGrade.h"
#include "SkillListEntry.generated.h"

class UButton;
class UImage;
class UTexture2D;
class UTextBlock;
class USkillBase;
class USkillTooltipWidget;

//등급 하나의 항목 박스 아트, 일반과 호버 2장
//클릭은 기능이 없으므로 Pressed 아트를 따로 두지 않고 hovered를 그 자리에도 쓴다
USTRUCT(BlueprintType)
struct FSkillEntryGradeArt
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "SkillEntry")
	TObjectPtr<UTexture2D> normal = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "SkillEntry")
	TObjectPtr<UTexture2D> hovered = nullptr;
};

//스킬 목록의 항목 하나, 아이콘과 이름만 표시한다. 액티브·패시브 공용
UCLASS()
class PW_API USkillListEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 내용 설정, 아이콘 미지정 스킬이면 아이콘을 숨긴다
	//count가 2 이상이면 이름 뒤에 "x N"을 붙인다
	//NativeConstruct보다 먼저 불려야 툴팁이 채워진다
	void InitEntry(USkillBase* skill, int32 count = 1);

protected:
	virtual void NativeConstruct() override;

	//누르는 용도가 아니라 호버를 잡는 히트테스트 표면이다, 클릭에 묶인 기능은 없다
	//루트가 SelfHitTestInvisible이면 이것 없이는 툴팁이 뜨지 않는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EntryButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> SkillIcon;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> SkillNameText;

	//이 항목의 툴팁으로 달 위젯 클래스, 띄우는 건 Slate가 한다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillTooltipWidget> skillTooltipClass;

	//등급별 EntryButton 아트, 비워 둔 등급은 디자이너가 넣어 둔 기본 브러시를 그대로 쓴다
	//UImageButton의 아트 기능은 버튼 콘텐츠가 UImage일 것을 요구해 여기서는 쓸 수 없다
	//(EntryButton의 콘텐츠는 HorizontalBox Row다) 그래서 FButtonStyle 브러시를 직접 갈아끼운다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TMap<EUpgradeGrade, FSkillEntryGradeArt> gradeArt;

private:
	//등급 아트를 EntryButton 브러시에 적용, Pressed에는 호버 아트를 넣는다
	void ApplyGradeArt(EUpgradeGrade grade);

	//툴팁에 넘길 스킬, 위젯이 사는 동안 GC로부터 보호한다
	UPROPERTY()
	TObjectPtr<USkillBase> entrySkill = nullptr;
};