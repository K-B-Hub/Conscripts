//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampDialogueWidget.h"
#include "Components/TextBlock.h"

void UCampDialogueWidget::SetLine(const FText& speaker, const FText& line)
{
	if (NameText) NameText->SetText(speaker);
	if (LineText) LineText->SetText(line);
}