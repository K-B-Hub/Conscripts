//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CampDialogueWidget.generated.h"

class UTextBlock;

//야영지 말풍선, 아군 머리 위 위젯 컴포넌트에 올라간다
//위 줄은 캐릭터 이름, 아래 줄은 대사
UCLASS()
class PW_API UCampDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetLine(const FText& speaker, const FText& line);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LineText;
};