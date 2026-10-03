//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenuWidget.generated.h"

class UButton;

//전투 중 ESC로 열리는 일시정지 메뉴
//게임 자체를 pause하므로 적 턴 타이머와 AI 이동이 함께 멈춘다
UCLASS()
class PW_API UPauseMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	//닫기(X), 전투로 복귀
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResumeButton;

	//부대 정보 화면, 아직 구현되지 않아 비활성으로 둔다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SquadInfoButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;

	//런을 유지한 채 허브로, 포기와 달리 로스터와 진행도가 남는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AbandonButton;

	//메인메뉴 복귀 확인, 런은 남지만 재개가 스테이지 시작부터라 현재 전투 진행이 날아간다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> MainMenuConfirmPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> MainMenuCancelButton;

	//런 포기 확인, 런 전체가 날아가는 되돌릴 수 없는 행동이라 한 번 거친다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> AbandonConfirmPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AbandonConfirmButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AbandonCancelButton;

private:
	//확인 패널 둘을 모두 접는다, 하나를 열 때 다른 하나가 남아 있으면 둘이 함께 보인다
	void CollapseConfirmPanels();

	UFUNCTION()
	void HandleResumeClicked();
	UFUNCTION()
	void HandleSettingsClicked();
	UFUNCTION()
	void HandleMainMenuClicked();
	UFUNCTION()
	void HandleMainMenuConfirmClicked();
	UFUNCTION()
	void HandleMainMenuCancelClicked();
	UFUNCTION()
	void HandleAbandonClicked();
	UFUNCTION()
	void HandleAbandonConfirmClicked();
	UFUNCTION()
	void HandleAbandonCancelClicked();
};