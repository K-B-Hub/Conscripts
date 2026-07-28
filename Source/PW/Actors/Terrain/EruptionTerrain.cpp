//Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/Terrain/EruptionTerrain.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "GameMode/BattleGameMode.h"

void AEruptionTerrain::BeginPlay()
{
	Super::BeginPlay();

	//기대 피해가 곧 손익, 분출은 체류자 본인도 휩쓸므로 음수
	aiBaseValue = -(eruptionChance / 100.f) * eruptionDamage;
}

void AEruptionTerrain::OnCharacterTurnStart(ACharacterBase* character)
{
	if (!character) return;

	if (FMath::RandRange(1, 100) > eruptionChance) return;

	ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>();
	if (!gm) return;

	//분출 중심은 턴을 시작한 체류자, 반경 내 전원이 대상이며 본인도 포함
	const FVector center = character->GetActorLocation();

	TArray<ACharacterBase*> affected;
	for (AAllyCharacterBase* ally : gm->GetAllies())
	{
		if (IsValid(ally) && !ally->IsDead()) affected.Add(ally);
	}
	for (AEnemyBase* enemy : gm->GetEnemies())
	{
		if (IsValid(enemy) && !enemy->IsDead()) affected.Add(enemy);
	}

	for (ACharacterBase* target : affected)
	{
		if (FVector::Dist(center, target->GetActorLocation()) > eruptionRadius) continue;

		//환경 피해는 비치명, hp 1까지만 깎임
		target->ReceiveDamage(eruptionDamage, false);
	}

	UE_LOG(LogTemp, Log, TEXT("[Terrain] %s 분출, 중심 %s"),
		*GetName(), *GetNameSafe(character));
}