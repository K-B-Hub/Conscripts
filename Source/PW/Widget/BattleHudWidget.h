//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleHudWidget.generated.h"

class UButton;
class UHoverInfoWidget;
class ACharacterBase;

//전투 내내 유지되는 HUD 컨테이너, 전투 페이즈 진입에서 생성되고 전투 종료에서 제거된다
//턴마다 생성·파괴되는 UBattleTurnWidget과 달리 적 턴에도 살아 있어야 하는 조각을 담는다
//임무 표시·턴 순서·일시정지 버튼·호버 미리보기가 단계별로 자식으로 들어온다
UCLASS()
class PW_API UBattleHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//호버 대상 변경 통지, nullptr이면 미리보기를 접는다
	void SetHoverTarget(ACharacterBase* target);

protected:
	virtual void NativeConstruct() override;

	//BP에서 "PauseButton" 이름으로 바인딩, 화면 우측 상단
	//임무 표시는 C++ 접근이 불필요해 바인딩 없이 BP에 자유 배치한다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PauseButton;

	//BP에서 "HoverInfo" 이름으로 바인딩, 화면 중앙 상단
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UHoverInfoWidget> HoverInfo;

private:
	UFUNCTION()
	void HandlePauseClicked();
};