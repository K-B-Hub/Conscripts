//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampRecruitWidget.generated.h"

class UPanelWidget;
class UTextBlock;
class UJobSelectButton;
class AAllyCharacterBase;

//고른 캐릭터 클래스를 통지, 후보가 걸러져 표시되므로 목록 인덱스로는 특정할 수 없다
DECLARE_DELEGATE_OneParam(FOnCharacterChosen, TSubclassOf<AAllyCharacterBase>);

//충원 시 캐릭터를 고르는 화면
//한 번 고를 때마다 한 명이 합류하므로 남은 인원만큼 반복해서 열린다
//야영지 충원과 전투 중 증원이 같은 위젯을 쓰므로 컨트롤러를 알지 않고 델리게이트로 통지한다
//후보 선별은 화면마다 다르므로(야영지는 로스터, 전투는 참전 인원) 띄우는 쪽이 목록을 넘긴다
UCLASS()
class PW_API UCampRecruitWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//고를 수 있는 캐릭터만 담아 목록을 다시 만든다
	//연속 충원 중 방금 합류한 캐릭터가 후보에 남지 않도록 매번 다시 호출한다
	void SetCandidates(const TArray<TSubclassOf<AAllyCharacterBase>>& candidates);

	//남은 충원 인원 표시 갱신
	void SetRemaining(int32 remaining);

	//캐릭터 선택 통지, 위젯을 띄운 컨트롤러가 바인딩
	FOnCharacterChosen OnCharacterChosen;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UPanelWidget> JobContainer;

	//"남은 인원 2명" 같은 안내
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> RemainingText;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UJobSelectButton> jobButtonClass;

private:
	void HandleCharacterClicked(TSubclassOf<AAllyCharacterBase> characterClass);
};
