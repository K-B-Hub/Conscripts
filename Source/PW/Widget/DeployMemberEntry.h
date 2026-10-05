//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeployMemberEntry.generated.h"

class UButton;
class UImage;
class UProgressBar;
class UTextBlock;
class UTexture2D;
struct FAllyRunState;

//항목 클릭 시 로스터 인덱스를 통지
DECLARE_DELEGATE_OneParam(FOnDeployMemberClicked, int32);

//출격 배치 화면의 로스터 항목 하나, 직업 이미지·이름·레벨·체력 바를 표시한다
UCLASS()
class PW_API UDeployMemberEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	//로스터 스냅샷으로 표시 내용 설정, 직업 정보는 CDO에서 읽어 상위 위젯이 넘긴다
	void InitEntry(const FAllyRunState& state, UTexture2D* jobIcon, int32 index);

	//선택·배치 여부 표시 갱신
	void SetEntryState(bool bSelected, bool bDeployed);

	//클릭 통지, 배치 위젯이 바인딩
	FOnDeployMemberClicked OnClicked;

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EntryButton;

	//개체 이름
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	//"LV. n"
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LevelText;

	//직업 이미지, 지정되지 않은 직업이면 숨긴다
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

private:
	UFUNCTION()
	void HandleClicked();

	//통지할 로스터 인덱스
	int32 entryIndex = INDEX_NONE;
};
