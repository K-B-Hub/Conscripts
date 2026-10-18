// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillWidget.generated.h"

class UUniformGridPanel;
class UButton;
class USizeBox;
class USkillButton;
class UActiveSkillBase;
class AAllyCharacterBase;

//턴 시작 시 생성되어 액티브 스킬 버튼을 8열 그리드에 동적으로 배치하는 위젯
//편집 모드의 슬롯 교환과 줄 추가도 이 클래스가 쥔다
UCLASS()
class PW_API USkillWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//캐릭터의 저장된 배치와 보유 스킬을 맞춰 그리드 칸 전체를 채운다, 빈 칸도 버튼이다
	void InitSkills(AAllyCharacterBase* unit);

	//모든 스킬 버튼의 활성/비활성 상태 갱신
	void RefreshButtons();

protected:
	virtual void NativeConstruct() override;

	//BP에서 "SkillGridPanel" 이름의 UniformGridPanel과 자동 바인딩
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> SkillGridPanel;

	//편집 모드 토글 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> EditModeButton;

	//편집 모드에서만 듣는 줄 추가 버튼
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> AddRowButton;

	//줄 추가 버튼을 호버 아트 크기로 고정하는 상자, 숨길 때는 자리까지 비우도록 이쪽을 접는다
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> AddRowBox;

	//스킬 버튼 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<USkillButton> skillButtonClass;

	//그리드 열 수, 슬롯 i는 (i / columns, i % columns)에 놓인다
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 columns = 8;

	//스킬이 적어도 유지할 최소 줄 수
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	int32 minRows = 3;

private:
	UFUNCTION()
	void OnEditModeClicked();

	UFUNCTION()
	void OnAddRowClicked();

	//평소면 스킬 사용, 편집 중이면 두 칸 교환
	void HandleSlotClicked(int32 clickedIndex);

	//보유 스킬과 저장된 배치를 맞춘다, 결과를 캐릭터에 되쓴다
	void NormalizeSlots();

	//칸 전체를 다시 만든다
	void RebuildGrid();

	void SwapSlots(int32 a, int32 b);

	//꼬리에서부터 빈 줄만 잘라낸다, 최소 줄 수는 보장
	void TrimTrailingRows();

	TWeakObjectPtr<AAllyCharacterBase> ownerUnit;

	//슬롯에 놓인 스킬 인스턴스, 빈 칸은 nullptr. 인덱스가 캐릭터의 클래스 배열과 1:1
	UPROPERTY()
	TArray<TObjectPtr<UActiveSkillBase>> slotSkills;

	//생성된 스킬 버튼 목록, 인덱스가 슬롯과 1:1
	UPROPERTY()
	TArray<TObjectPtr<USkillButton>> skillButtons;

	bool bEditMode = false;

	//교환 대기 중인 첫 번째 칸
	int32 selectedSlotIndex = INDEX_NONE;
};