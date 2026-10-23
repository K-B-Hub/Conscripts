//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResultMemberEntry.generated.h"

class UButton;
class UImage;
class UProgressBar;
class UTextBlock;
class UCharacterTooltipWidget;
class AAllyCharacterBase;
struct FAllyRunState;

//항목 클릭 시 자신이 쥔 캐릭터 클래스를 통지, 결과 화면이 상세 정보 창을 여는 데 쓴다
DECLARE_DELEGATE_OneParam(FOnResultMemberClicked, TSubclassOf<AAllyCharacterBase>);

//전투 결과 화면의 생존자 한 줄
//칸은 전부 선택 바인딩이라 WBP에 넣은 것만 채워진다
UCLASS()
class PW_API UResultMemberEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	//로스터 스냅샷으로 표시 내용 설정
	//이름·직업·초상화·소개는 스냅샷의 캐릭터 클래스 CDO에서 직접 읽는다
	void InitEntry(const FAllyRunState& state);

	//클릭 통지, 목록을 만든 쪽이 바인딩
	FOnResultMemberClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> EntryButton;

	//캐릭터 초상화, 고유 초상화가 없으면 직업 아이콘으로 떨어지고 둘 다 없으면 숨긴다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> PortraitImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobText;

	//"LV. n"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	//"11 / 20" 체력 수치와 바, 전투를 얼마나 상한 채 돌아왔는지 보여준다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HpText;
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HpBar;

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
