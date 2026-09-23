//Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/CampDialogueWidget.h"
#include "Components/TextBlock.h"

void UCampDialogueWidget::SetLine(const FText& line)
{
	if (LineText) LineText->SetText(line);
}