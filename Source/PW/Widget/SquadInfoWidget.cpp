//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SquadInfoWidget.h"
#include "Widget/SquadMemberRow.h"
#include "Characters/AllyCharacterBase.h"
#include "Components/Button.h"
#include "Components/PanelWidget.h"

void USquadInfoWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (CloseButton) CloseButton->OnClicked.AddDynamic(this, &USquadInfoWidget::HandleCloseClicked);
}

void USquadInfoWidget::InitSquad(const TArray<TObjectPtr<AAllyCharacterBase>>& members)
{
	if (!MemberList || !rowClass) return;

	MemberList->ClearChildren();

	for (const TObjectPtr<AAllyCharacterBase>& member : members)
	{
		//전투 중 사망자는 GameMode 배열에 남아 있을 수 있다, 표에서는 제외한다
		if (!IsValid(member) || member->IsDead()) continue;

		USquadMemberRow* row = CreateWidget<USquadMemberRow>(this, rowClass);
		if (!row) continue;

		row->InitRow(member);
		row->OnClicked.BindUObject(this, &USquadInfoWidget::HandleRowClicked);
		MemberList->AddChild(row);
	}
}

void USquadInfoWidget::HandleCloseClicked()
{
	OnClosed.ExecuteIfBound();
}

void USquadInfoWidget::HandleRowClicked(ACharacterBase* target)
{
	OnMemberSelected.ExecuteIfBound(target);
}
