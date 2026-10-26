//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/High/Ailment/RifleStickyBombAilment.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "GameMode/BattleGameMode.h"

URifleStickyBombAilment::URifleStickyBombAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "RifleStickyBomb_Name", "점착 폭탄");
	ailmentDescription = NSLOCTEXT("Ailment", "RifleStickyBomb_Description", "다음 턴 시작에 폭발해 주변 3m의 모든 캐릭터에게 피해를 준다");

	//다음 턴 시작에 터지고 그 턴 종료에 소멸한다
	ailmentTurn = 1;
	isStart = true;
	isEnd = false;
	isStackable = false;

	//시전자 공격력에 비례해 정적 환산이 불가, 초기 공격력 13 기준 대략값
	aiDamagePerTurn = 15.f;
	aiControlCoefficient = 0.f;
}

void URifleStickyBombAilment::OnApply(ACharacterBase* affected, ACharacterBase* caster)
{
	Super::OnApply(affected, caster);

	if (!caster) return;

	//폭발 시점에 시전자가 죽어 있을 수 있으므로 공격력을 값으로 스냅
	snappedDamage = caster->GetAtk() * damageRatio;
	snappedDamageMax = caster->GetAtk() * damageRatioMax;
}

void URifleStickyBombAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	const UWorld* world = affected->GetWorld();
	const ABattleGameMode* gameMode = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gameMode) return;

	//폭발은 한 번이므로 피해도 한 번만 굴려 휩쓸린 전원에게 같은 값을 적용한다
	const int32 amount = FMath::Max(1, FMath::RoundToInt(FMath::FRandRange(snappedDamage, snappedDamageMax)));

	//폭탄이 붙은 캐릭터를 포함해 반경 내 전원이 피아 구분 없이 휩쓸린다
	TArray<ACharacterBase*> caught;
	for (AAllyCharacterBase* ally : gameMode->GetAllies())
	{
		if (IsValid(ally) && !ally->IsDead()) caught.Add(ally);
	}
	for (AEnemyBase* enemy : gameMode->GetEnemies())
	{
		if (IsValid(enemy) && !enemy->IsDead()) caught.Add(enemy);
	}

	for (ACharacterBase* target : caught)
	{
		if (FVector::Dist2D(affected->GetActorLocation(), target->GetActorLocation()) > explodeRadius) continue;

		//상태이상 피해라 비치명, 체력 1까지만 깎이고 이 피해로는 죽지 않는다
		target->ReceiveDamage(amount, false);
	}
}
