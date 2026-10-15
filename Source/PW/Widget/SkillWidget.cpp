// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/SkillWidget.h"
#include "Widget/SkillButton.h"
#include "Characters/AllyCharacterBase.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "PlayerController/BattleController.h"
#include "Components/Button.h"
#include "Components/UniformGridPanel.h"

void USkillWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditModeButton)
	{
		EditModeButton->OnClicked.AddDynamic(this, &USkillWidget::OnEditModeClicked);
	}

	if (AddRowButton)
	{
		AddRowButton->OnClicked.AddDynamic(this, &USkillWidget::OnAddRowClicked);

		//편집 모드에서만 듣는 버튼이라 평소에는 숨긴다
		AddRowButton->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void USkillWidget::InitSkills(AAllyCharacterBase* unit)
{
	if (!IsValid(unit) || !unit->GetSkillComponent()) return;
	if (!SkillGridPanel || !skillButtonClass || columns <= 0) return;

	ownerUnit = unit;

	//턴마다 평소 모드로 시작한다
	bEditMode = false;
	selectedSlotIndex = INDEX_NONE;

	NormalizeSlots();
	RebuildGrid();
}

void USkillWidget::NormalizeSlots()
{
	AAllyCharacterBase* unit = ownerUnit.Get();
	if (!unit) return;

	USkillComponent* comp = unit->GetSkillComponent();
	if (!comp) return;

	TArray<UActiveSkillBase*> owned = comp->GetActiveSkills();
	TArray<TSubclassOf<USkillBase>> slots = unit->GetSkillSlots();

	//스킬을 다 담을 줄 수, 최소 minRows는 비어 있어도 유지한다
	const int32 need = FMath::Max(columns * minRows,
		FMath::DivideAndRoundUp(owned.Num(), columns) * columns);
	if (slots.Num() < need)
	{
		slots.SetNum(need);
	}

	slotSkills.Init(nullptr, slots.Num());

	TArray<bool> placed;
	placed.Init(false, owned.Num());

	//1패스 — 저장된 칸 복원. 잃은 스킬의 칸은 비우고 뒤를 당겨오지 않는다
	for (int32 i = 0; i < slots.Num(); ++i)
	{
		if (!slots[i]) continue;

		for (int32 s = 0; s < owned.Num(); ++s)
		{
			if (placed[s] || !owned[s] || owned[s]->GetClass() != slots[i]) continue;

			slotSkills[i] = owned[s];
			placed[s] = true;
			break;
		}

		if (!slotSkills[i])
		{
			slots[i] = nullptr;
		}
	}

	//2패스 — 미배치 스킬을 가장 앞 빈 칸에 꽂는다. 순서를 뒤집으면 신규가 저장된 자리를 선점한다
	for (int32 s = 0; s < owned.Num(); ++s)
	{
		if (placed[s] || !owned[s]) continue;

		int32 target = INDEX_NONE;
		for (int32 i = 0; i < slotSkills.Num(); ++i)
		{
			if (!slotSkills[i])
			{
				target = i;
				break;
			}
		}

		//빈 칸이 없으면 줄을 하나 늘린다, 자리가 없다고 버리면 습득한 스킬이 사라진다
		if (target == INDEX_NONE)
		{
			target = slots.Num();
			slots.SetNum(slots.Num() + columns);
			slotSkills.SetNum(slots.Num());
		}

		slots[target] = owned[s]->GetClass();
		slotSkills[target] = owned[s];
		placed[s] = true;
	}

	unit->SetSkillSlots(slots);
}

void USkillWidget::RebuildGrid()
{
	if (!SkillGridPanel || !skillButtonClass || columns <= 0) return;

	SkillGridPanel->ClearChildren();
	skillButtons.Empty();

	for (int32 i = 0; i < slotSkills.Num(); ++i)
	{
		USkillButton* button = CreateWidget<USkillButton>(GetOwningPlayer(), skillButtonClass);
		if (!button) continue;

		button->SetSlotIndex(i);
		button->OnSlotClicked.BindUObject(this, &USkillWidget::HandleSlotClicked);

		//AddChild가 NativeConstruct를 부르므로 InitSkill이 먼저 와야 툴팁이 채워진다
		button->InitSkill(slotSkills[i]);
		button->SetEditMode(bEditMode);
		button->SetSelected(i == selectedSlotIndex);

		SkillGridPanel->AddChildToUniformGrid(button, i / columns, i % columns);
		skillButtons.Add(button);
	}
}

void USkillWidget::RefreshButtons()
{
	for (USkillButton* Button : skillButtons)
	{
		if (IsValid(Button))
		{
			Button->RefreshButtonState();
		}
	}
}

void USkillWidget::HandleSlotClicked(int32 clickedIndex)
{
	if (!slotSkills.IsValidIndex(clickedIndex)) return;

	if (!bEditMode)
	{
		if (UActiveSkillBase* skill = slotSkills[clickedIndex])
		{
			if (ABattleController* BC = Cast<ABattleController>(GetOwningPlayer()))
			{
				BC->ActivateSkill(skill);
			}
		}
		return;
	}

	if (selectedSlotIndex == INDEX_NONE)
	{
		selectedSlotIndex = clickedIndex;
		if (skillButtons.IsValidIndex(clickedIndex) && IsValid(skillButtons[clickedIndex]))
		{
			skillButtons[clickedIndex]->SetSelected(true);
		}
		return;
	}

	//같은 칸을 다시 누르면 선택만 해제한다
	if (selectedSlotIndex == clickedIndex)
	{
		if (skillButtons.IsValidIndex(clickedIndex) && IsValid(skillButtons[clickedIndex]))
		{
			skillButtons[clickedIndex]->SetSelected(false);
		}
		selectedSlotIndex = INDEX_NONE;
		return;
	}

	SwapSlots(selectedSlotIndex, clickedIndex);
	selectedSlotIndex = INDEX_NONE;
}

void USkillWidget::SwapSlots(int32 a, int32 b)
{
	if (!slotSkills.IsValidIndex(a) || !slotSkills.IsValidIndex(b)) return;

	slotSkills.Swap(a, b);

	//캐릭터의 클래스 배열도 같은 순서로 맞춘다, 이쪽이 저장되는 배치다
	if (AAllyCharacterBase* unit = ownerUnit.Get())
	{
		TArray<TSubclassOf<USkillBase>> slots = unit->GetSkillSlots();
		if (slots.IsValidIndex(a) && slots.IsValidIndex(b))
		{
			slots.Swap(a, b);
			unit->SetSkillSlots(slots);
		}
	}

	//칸을 다시 만들지 않고 둘만 갱신한다, InitSkill이 툴팁까지 교환에 맞춰 바꾼다
	for (int32 index : { a, b })
	{
		if (!skillButtons.IsValidIndex(index) || !IsValid(skillButtons[index])) continue;

		skillButtons[index]->InitSkill(slotSkills[index]);
		skillButtons[index]->SetSelected(false);
		skillButtons[index]->RefreshButtonState();
	}
}

void USkillWidget::OnEditModeClicked()
{
	bEditMode = !bEditMode;

	if (AddRowButton)
	{
		AddRowButton->SetVisibility(bEditMode ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
	}

	if (bEditMode)
	{
		//쓸 수 없는 스킬도 집히게 전부 켠다
		for (USkillButton* button : skillButtons)
		{
			if (IsValid(button))
			{
				button->SetEditMode(true);
			}
		}
		return;
	}

	//나갈 때 한 번만 정리한다, 편집 중에 자르면 방금 추가한 줄이 사라진다
	TrimTrailingRows();
	selectedSlotIndex = INDEX_NONE;
	RebuildGrid();
}

void USkillWidget::OnAddRowClicked()
{
	if (!bEditMode || columns <= 0) return;

	AAllyCharacterBase* unit = ownerUnit.Get();
	if (!unit) return;

	TArray<TSubclassOf<USkillBase>> slots = unit->GetSkillSlots();
	slots.SetNum(slots.Num() + columns);
	unit->SetSkillSlots(slots);

	slotSkills.SetNum(slots.Num());
	RebuildGrid();
}

void USkillWidget::TrimTrailingRows()
{
	AAllyCharacterBase* unit = ownerUnit.Get();
	if (!unit || columns <= 0) return;

	TArray<TSubclassOf<USkillBase>> slots = unit->GetSkillSlots();
	const int32 minSlots = columns * minRows;

	//꼬리에서부터 줄 단위로 보며 스킬이 하나라도 있으면 즉시 멈춘다
	//"빈 줄을 전부 센다"로 구현하면 중간 빈 줄을 건너뛰고 더 앞까지 자른다
	while (slots.Num() > minSlots)
	{
		const int32 rowStart = slots.Num() - columns;

		bool bRowEmpty = true;
		for (int32 i = rowStart; i < slots.Num(); ++i)
		{
			if (slots[i])
			{
				bRowEmpty = false;
				break;
			}
		}

		if (!bRowEmpty) break;

		slots.SetNum(rowStart);
	}

	unit->SetSkillSlots(slots);
	slotSkills.SetNum(slots.Num());
}