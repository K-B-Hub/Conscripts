//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeployMemberEntry.generated.h"

class UButton;
class UImage;
class UProgressBar;
class UTextBlock;
class UCharacterTooltipWidget;
struct FAllyRunState;

//항목 클릭 시 로스터 인덱스를 통지
DECLARE_DELEGATE_OneParam(FOnDeployMemberClicked, int32);

//출격 배치 화면의 로스터 항목 하나, 초상화·이름·레벨·체력 바를 표시한다
UCLASS()
class PW_API UDeployMemberEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	//로스터 스냅샷으로 표시 내용 설정
	//이름·초상화·소개는 스냅샷의 캐릭터 클래스 CDO에서 직접 읽는다
	void InitEntry(const FAllyRunState& state, int32 index);

	//선택·배치 여부 표시 갱신
	void SetEntryState(bool bSelected, bool bDeployed);

	//클릭 통지, 배치 위젯이 바인딩
	FOnDeployMemberClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EntryButton;

	//캐릭터 고유 이름, 직업 칸과 따로 두어 둘 중 하나만 쓰는 배치도 가능하다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> NameText;

	//"LV. n"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	//직업 이름
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobText;

	//캐릭터 초상화, 고유 초상화가 없으면 직업 아이콘으로 떨어지고 둘 다 없으면 숨긴다
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> JobImage;

	//현재 / 최대 체력
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UProgressBar> HpBar;

	//"현재 / 최대"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> HpText;

	//선택 시 표시할 강조 테두리 등
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> SelectedHighlight;

	//배치 완료 시 표시할 표식
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> DeployedMark;

	//소개 툴팁 위젯, 미지정이면 툴팁 없이 동작한다
	UPROPERTY(EditDefaultsOnly, Category = "Tooltip")
	TSubclassOf<UCharacterTooltipWidget> characterTooltipClass;

private:
	UFUNCTION()
	void HandleClicked();

	//통지할 로스터 인덱스
	int32 entryIndex = INDEX_NONE;
};
