//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterTooltipWidget.generated.h"

class UImage;
class UTextBlock;
class ACharacterBase;

//캐릭터 초상화·이름·직업·소개를 보여주는 툴팁
//부대 정보표와 편성·증원 화면이 같은 위젯을 쓴다
//CDO도 ACharacterBase*이므로 클래스만 쥔 화면은 GetCDO 결과를 그대로 넘기면 된다
//레벨·체력 같은 런타임 값은 담지 않는다 — CDO로도 같은 함수를 부를 수 있어야 한다
UCLASS()
class PW_API UCharacterTooltipWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//표시 내용을 채운다, 내용이 바뀌지 않으므로 부착 시 한 번만 호출한다
	void InitTooltip(const ACharacterBase* character);

protected:
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> PortraitImage;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> CharacterNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> JobNameText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> DescriptionText;
};
