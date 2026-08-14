//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Rest/RestBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "GameMode/BattleGameMode.h"

void URestBase::Execute(ACharacterBase* instigator)
{
	//파생 클래스에서 구현
}

void URestBase::CollectTeam(const ACharacterBase* instigator, TArray<ACharacterBase*>& outTeam)
{
	if (!instigator) return;

	const UWorld* world = instigator->GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	if (instigator->IsAlly())
	{
		for (AAllyCharacterBase* ally : gm->GetAllies())
		{
			if (ally && !ally->IsDead()) outTeam.Add(ally);
		}
		return;
	}

	for (AEnemyBase* enemy : gm->GetEnemies())
	{
		if (enemy && !enemy->IsDead()) outTeam.Add(enemy);
	}
}

int32 URestBase::CalcRatioAmount(int32 base, float ratio)
{
	if (base <= 0 || ratio <= 0.f) return 0;

	//반올림 후 최소 1 보장, 비율이 작아도 효과가 없는 강화가 되지 않도록
	return FMath::Max(1, FMath::RoundToInt(base * ratio));
}