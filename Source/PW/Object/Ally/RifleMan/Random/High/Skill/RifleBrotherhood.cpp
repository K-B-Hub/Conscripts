//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Skill/RifleBrotherhood.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "GameMode/BattleGameMode.h"

URifleBrotherhood::URifleBrotherhood()
{
	skillName = NSLOCTEXT("Skill", "RifleBrotherhood_Name", "전우애");
	skillDescription = NSLOCTEXT("Skill", "RifleBrotherhood_Description", "주변 3m 내의 아군 1명당 명중과 회피가 5 증가한다(최대 20)");
	skillGrade = EUpgradeGrade::High;

	//누가 이동을 마칠 때마다 주변 인원이 바뀌므로 MoveComplete로 재평가
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//스택을 인스턴스가 추적하므로 중복 획득 불가
	bAllowDuplicate = false;

	//1스택분, 실제 증가량은 스택 수에 비례한다
	accuracy = 5.f;
	evasion = 5.f;
}

void URifleBrotherhood::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	const ABattleGameMode* gameMode = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gameMode) return;

	//사거리 안의 살아있는 아군 수, 자신은 제외
	int32 nearbyCount = 0;
	for (const AAllyCharacterBase* ally : gameMode->GetAllies())
	{
		if (!IsValid(ally) || ally == o || ally->IsDead()) continue;
		if (FVector::Dist2D(o->GetActorLocation(), ally->GetActorLocation()) <= auraRange)
		{
			++nearbyCount;
		}
	}

	const int32 targetStack = FMath::Min(nearbyCount, maxStack);
	const int32 delta = targetStack - appliedStack;
	if (delta == 0) return;

	//ApplyPassiveStatDelta는 sign ±1만 받으므로 차이만큼 반복 호출
	const int32 sign = (delta > 0) ? 1 : -1;
	for (int32 i = 0; i < FMath::Abs(delta); ++i)
	{
		o->ApplyPassiveStatDelta(this, sign);
	}
	appliedStack = targetStack;
}
