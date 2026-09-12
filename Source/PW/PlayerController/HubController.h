//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Enum/GameDifficulty.h"
#include "Run/AllyRunState.h"
#include "HubController.generated.h"

class UUserWidget;
class UMainMenuWidget;
class UModeSelectWidget;
class UStoryRouteSelectWidget;
class USettingsWidget;
class UFormationWidget;

//허브 레벨의 화면 전환을 전담, 화면은 항상 하나만 살아 있다
//위젯은 GetOwningPlayer로 이 클래스를 직접 호출하고, 다음 화면 결정은 여기서 한다
UCLASS()
class PW_API AHubController : public APlayerController
{
	GENERATED_BODY()

public:
	void ShowMainMenu();
	void ShowModeSelect();
	void ShowStoryRouteSelect();
	void ShowSettings();
	void ShowFormation();

	//모드 확정, 스토리는 줄기 선택으로 나머지는 편성으로 진행
	void ChooseMode(EGameDifficulty mode);

	//스토리 줄기 확정, 편성으로 진행
	void ChooseStoryRoute(FName routeId);

	//편성 확정, 런을 열고 첫 스테이지로 진입
	void ConfirmFormation(const TArray<FAllyRunState>& roster);

	//편성 화면이 스토리 고정 편성을 조회할 때 사용, 스토리가 아니면 NAME_None
	FName GetPendingStoryRouteId() const { return pendingStoryRouteId; }

	void QuitGame();

protected:
	virtual void BeginPlay() override;

	//화면별 위젯 클래스, BP에서 지정
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMainMenuWidget> mainMenuWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UModeSelectWidget> modeSelectWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UStoryRouteSelectWidget> storyRouteSelectWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USettingsWidget> settingsWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UFormationWidget> formationWidgetClass;

private:
	//기존 화면을 제거하고 새 위젯을 생성해 표시
	void SwapScreen(TSubclassOf<UUserWidget> widgetClass);

	//현재 표시 중인 화면, 전환 시 제거 대상
	UPROPERTY()
	TObjectPtr<UUserWidget> currentWidget = nullptr;

	//줄기 선택 화면에서 고른 줄기, 편성 확정 시 StartRun으로 넘긴다
	//런은 편성 확정 시점에 열리므로 그 전까지는 화면 흐름을 쥔 이 클래스가 들고 있는다
	FName pendingStoryRouteId = NAME_None;
};