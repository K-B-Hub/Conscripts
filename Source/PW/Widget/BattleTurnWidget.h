// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BattleTurnWidget.generated.h"

class UMoveWidget;
class USkillWidget;
class UCircularGaugeWidget;
class AAllyCharacterBase;

//아군 턴 동안만 표시되는 HUD 컨테이너, 이동/스킬/턴종료/게이지 위젯을 하나로 묶음
//TurnEndWidget은 C++ 접근이 불필요해 바인딩 없이 BP에 자유 배치
UCLASS()
class PW_API UBattleTurnWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//턴 시작 시 현재 턴 유닛 기준으로 자식 위젯 일괄 초기화
	void InitTurnHUD(AAllyCharacterBase* TurnUnit);

	//이동력 잔여 여부로 이동 버튼 갱신
	void RefreshMoveButton(float currentMovingPoint);

	//스킬 버튼 활성/비활성 갱신
	void RefreshSkillButtons();

	//HP/스트레스 게이지 갱신, 레벨업·패시브로 최대치도 변하므로 max까지 함께 반영
	void RefreshGauges(int32 InMaxHp, int32 InHp, int32 InMaxStress, int32 InStress);

protected:
	//BP에서 각 이름으로 자동 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMoveWidget> MoveWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USkillWidget> SkillWidget;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCircularGaugeWidget> HpGauge;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCircularGaugeWidget> StressGauge;
};
