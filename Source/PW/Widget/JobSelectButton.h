//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "JobSelectButton.generated.h"

class UButton;
class UImage;
class UTextBlock;
class UCharacterTooltipWidget;
class AAllyCharacterBase;

//항목 클릭 시 자신이 쥔 캐릭터 클래스를 통지
//후보 목록이 걸러져 표시되므로 목록 인덱스로는 캐릭터를 특정할 수 없다
//같은 캐릭터를 두 명 둘 수 없어 클래스 하나가 곧 한 명을 가리킨다
DECLARE_DELEGATE_OneParam(FOnCharacterButtonClicked, TSubclassOf<AAllyCharacterBase>);

//편성·증원 화면의 목록 항목 하나, 캐릭터 후보와 편성된 인원 양쪽에 쓰인다
//표시 내용은 전부 캐릭터 CDO에서 읽으므로 어느 목록에 속하는지는 상위 위젯만 안다
UCLASS()
class PW_API UJobSelectButton : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 내용을 채우고 소개 툴팁을 붙인다
	void InitEntry(TSubclassOf<AAllyCharacterBase> characterClass);

	//항목에 덧붙일 정보, 목록마다 다르다 — 전투 결과는 레벨·체력을 함께 보여준다
	//CDO에 없는 런타임 수치라 InitEntry가 채울 수 없다
	void SetDetailText(const FText& detail);

	//클릭 통지, 목록을 만든 쪽이 바인딩
	FOnCharacterButtonClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> JobButton;

	//캐릭터 이름, 직업명은 JobText가 따로 보여준다
	//이름 없이 초상화만 쓰는 배치도 있으므로 선택 바인딩이다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LabelText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobText;

	//SetDetailText를 부르지 않는 목록에서는 접힌 채로 남는다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DetailText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> PortraitImage;

	//소개 툴팁 위젯, 미지정이면 툴팁 없이 동작한다
	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<UCharacterTooltipWidget> characterTooltipClass;

private:
	UFUNCTION()
	void HandleClicked();

	//통지할 캐릭터 클래스
	UPROPERTY()
	TSubclassOf<AAllyCharacterBase> entryCharacterClass;
};
