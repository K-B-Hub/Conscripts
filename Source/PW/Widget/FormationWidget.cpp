//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/FormationWidget.h"
#include "Widget/JobSelectButton.h"
#include "PlayerController/HubController.h"
#include "GameInstance/PWGameInstance.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"
#include "Components/TextBlock.h"

void UFormationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ConfirmButton) ConfirmButton->OnClicked.AddDynamic(this, &UFormationWidget::HandleConfirmClicked);
	if (BackButton)    BackButton->OnClicked.AddDynamic(this, &UFormationWidget::HandleBackClicked);

	SetupForMode();
	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UFormationWidget::SetupForMode()
{
	const UWorld* world = GetWorld();
	const UPWGameInstance* gameInstance = world ? world->GetGameInstance<UPWGameInstance>() : nullptr;
	if (!gameInstance) return;

	const AHubController* hub = GetOwningPlayer<AHubController>();
	storyRouteId = hub ? hub->GetPendingStoryRouteId() : NAME_None;

	//스토리는 줄기가 정한 인물들, 나머지는 해금된 캐릭터 — 고르는 방식 자체는 같다
	characterPool = gameInstance->GetCharacterPool(storyRouteId);

	//후보가 정원보다 적으면 채울 수 없는 화면에 갇히므로 정원을 후보 수로 줄인다
	rosterCapacity = FMath::Min(gameInstance->GetInitialRosterSize(), characterPool.Num());
}

void UFormationWidget::BuildJobList()
{
	if (!JobContainer || !jobButtonClass) return;

	JobContainer->ClearChildren();

	for (const TSubclassOf<AAllyCharacterBase>& characterClass : characterPool)
	{
		//이미 편성한 캐릭터는 후보에서 빼 중복 선택 자체를 없앤다
		if (IndexOfPending(characterClass) != INDEX_NONE) continue;

		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(characterClass);
		entry->OnClicked.BindUObject(this, &UFormationWidget::HandleCharacterClicked);
		JobContainer->AddChild(entry);
	}
}

void UFormationWidget::BuildRosterList()
{
	if (!RosterContainer || !jobButtonClass) return;

	RosterContainer->ClearChildren();

	for (const FAllyRunState& member : pendingRoster)
	{
		UJobSelectButton* entry = CreateWidget<UJobSelectButton>(this, jobButtonClass);
		if (!entry) continue;

		entry->InitEntry(member.AllyClass);
		entry->OnClicked.BindUObject(this, &UFormationWidget::HandleRosterClicked);
		RosterContainer->AddChild(entry);
	}
}

void UFormationWidget::RefreshConfirmState()
{
	const bool bFilled = pendingRoster.Num() == rosterCapacity && rosterCapacity > 0;

	if (CountText)
	{
		CountText->SetText(FText::FromString(FString::Printf(TEXT("%d / %d"), pendingRoster.Num(), rosterCapacity)));
	}
	if (ConfirmButton)
	{
		ConfirmButton->SetIsEnabled(bFilled);
	}
}

void UFormationWidget::HandleCharacterClicked(TSubclassOf<AAllyCharacterBase> characterClass)
{
	if (pendingRoster.Num() >= rosterCapacity) return;

	//후보 밖이거나 이미 편성된 캐릭터는 받지 않는다, 목록이 걸러도 통지는 들어올 수 있다
	if (!characterPool.Contains(characterClass)) return;
	if (IndexOfPending(characterClass) != INDEX_NONE) return;

	pendingRoster.Add(FAllyRunState::MakeFromClass(characterClass));

	//편성한 캐릭터가 후보에서 빠지므로 후보 목록도 다시 만든다
	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UFormationWidget::HandleRosterClicked(TSubclassOf<AAllyCharacterBase> characterClass)
{
	const int32 index = IndexOfPending(characterClass);
	if (index == INDEX_NONE) return;

	pendingRoster.RemoveAt(index);

	//해제한 캐릭터는 후보로 돌아온다
	BuildJobList();
	BuildRosterList();
	RefreshConfirmState();
}

void UFormationWidget::HandleConfirmClicked()
{
	if (AHubController* hub = GetOwningPlayer<AHubController>())
	{
		hub->ConfirmFormation(pendingRoster);
	}
}

void UFormationWidget::HandleBackClicked()
{
	AHubController* hub = GetOwningPlayer<AHubController>();
	if (!hub) return;

	//스토리는 줄기 선택에서 왔고 나머지는 모드 선택에서 왔다
	if (storyRouteId.IsNone()) hub->ShowModeSelect();
	else                       hub->ShowStoryRouteSelect();
}

int32 UFormationWidget::IndexOfPending(TSubclassOf<AAllyCharacterBase> characterClass) const
{
	if (!characterClass) return INDEX_NONE;

	for (int32 i = 0; i < pendingRoster.Num(); ++i)
	{
		if (pendingRoster[i].AllyClass == characterClass) return i;
	}
	return INDEX_NONE;
}
