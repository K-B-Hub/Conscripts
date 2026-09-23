//Fill out your copyright notice in the Description page of Project Settings.

#include "DataAsset/CampDialogueData.h"
#include "Characters/AllyCharacterBase.h"

namespace
{
	//비어 있지 않으면 무작위 한 줄, 비었으면 빈 텍스트
	FText PickRandom(const TArray<FText>& lines)
	{
		if (lines.Num() == 0) return FText::GetEmpty();

		return lines[FMath::RandRange(0, lines.Num() - 1)];
	}
}

FText UCampDialogueData::PickLine(const AAllyCharacterBase* ally) const
{
	if (!ally) return FText::GetEmpty();

	//부상이 스트레스보다 우선, 둘 다 해당하면 몸 상태를 먼저 말한다
	const int32 maxHp = FMath::Max(1, ally->GetMaxHp());
	if (static_cast<float>(ally->GetHp()) / maxHp <= woundedHpRatio)
	{
		const FText line = PickRandom(woundedLines);
		if (!line.IsEmpty()) return line;
	}

	const int32 maxStress = FMath::Max(1, ally->GetMaxStress());
	if (static_cast<float>(ally->GetStress()) / maxStress >= stressedRatio)
	{
		const FText line = PickRandom(stressedLines);
		if (!line.IsEmpty()) return line;
	}

	return PickRandom(healthyLines);
}