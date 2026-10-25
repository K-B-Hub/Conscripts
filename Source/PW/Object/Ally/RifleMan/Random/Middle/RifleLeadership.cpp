//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/RifleLeadership.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"

URifleLeadership::URifleLeadership()
{
	skillName = NSLOCTEXT("Skill", "RifleLeadership_Name", "리더십");
	skillDescription = NSLOCTEXT("Skill", "RifleLeadership_Description", "주변 5m 내의 모든 아군에게 공격력을 2 부여한다");
	skillGrade = EUpgradeGrade::Mid;

	//누가 이동을 마칠 때마다 주변 인원이 바뀌므로 MoveComplete로 재평가
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//적용 대상을 인스턴스가 추적하므로 중복 획득 불가
	bAllowDuplicate = false;

	atk = 2;
}

void URifleLeadership::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	ABattleGameMode* gameMode = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gameMode) return;

	//사거리 안의 살아있는 아군 수집, 시전자 자신은 제외
	TArray<ACharacterBase*> inRange;
	for (AAllyCharacterBase* ally : gameMode->GetAllies())
	{
		if (!IsValid(ally) || ally == o || ally->IsDead()) continue;
		if (FVector::Dist2D(o->GetActorLocation(), ally->GetActorLocation()) <= auraRange)
		{
			inRange.Add(ally);
		}
	}

	//범위를 벗어났거나 사라진 대상에서 먼저 회수, 목록이 줄어드므로 역순 순회
	for (int32 i = buffedAllies.Num() - 1; i >= 0; --i)
	{
		ACharacterBase* previous = buffedAllies[i].Get();
		const bool bStillValid = IsValid(previous) && !previous->IsDead();
		if (bStillValid && inRange.Contains(previous)) continue;

		//죽었거나 파괴된 대상은 되돌릴 스탯도 없다
		if (bStillValid) previous->ApplyPassiveStatDelta(this, -1);
		buffedAllies.RemoveAt(i);
	}

	//새로 범위에 들어온 대상에 적용
	for (ACharacterBase* ally : inRange)
	{
		if (buffedAllies.Contains(ally)) continue;

		ally->ApplyPassiveStatDelta(this, +1);
		buffedAllies.Add(ally);
	}
}