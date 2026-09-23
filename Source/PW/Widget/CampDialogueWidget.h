//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampDialogueWidget.generated.h"

class UTextBlock;

//야영지 말풍선, 아군 머리 위 위젯 컴포넌트에 올라간다
//누가 말하는지는 위치로 드러나므로 대사만 담는다
UCLASS()
class PW_API UCampDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetLine(const FText& line);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LineText;
};