//Fill out your copyright notice in the Description page of Project Settings.

#include "DataAsset/CampOptionData.h"
#include "GameMode/CampGameMode.h"
#include "Characters/AllyCharacterBase.h"

namespace
{
	//최대치 비율을 정수량으로, 비율이 0이면 아무것도 하지 않는다
	int32 RatioAmount(int32 maxValue, float ratio)
	{
		if (maxValue <= 0 || ratio <= 0.f) return 0;

		//반올림 후 최소 1, 비율이 작아도 효과가 없는 선택지가 되지 않도록
		return FMath::Max(1, FMath::RoundToInt(maxValue * ratio));
	}
}

void UCampOptionData::ApplyRestoration(ACampGameMode* camp) const
{
	if (!camp) return;

	for (AAllyCharacterBase* ally : camp->GetCampAllies())
	{
		if (!IsValid(ally)) continue;

		ally->RestAtCamp(
			RatioAmount(ally->GetMaxHp(), hpRatio),
			RatioAmount(ally->GetMaxStress(), stressRatio));

		const int32 resource = RatioAmount(ally->GetMaxBattleResource(), battleResourceRatio);
		if (resource > 0) ally->GainBattleResource(resource);
	}
}