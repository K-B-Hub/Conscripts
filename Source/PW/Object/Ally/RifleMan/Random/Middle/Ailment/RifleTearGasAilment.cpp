//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Middle/Ailment/RifleTearGasAilment.h"
#include "Characters/CharacterBase.h"

URifleTearGasAilment::URifleTearGasAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "RifleTearGas_Name", "최루 가스");
	ailmentDescription = NSLOCTEXT("Ailment", "RifleTearGas_Description", "턴 시작마다 최대 체력의 10%에 해당하는 피해를 입는다");

	ailmentTurn = 2;
	isStart = true;
	isEnd = false;
	isStackable = false;

	//비율 피해라 정적 환산이 불가, 체력 40 기준 대략값
	aiDamagePerTurn = 4.f;
	aiControlCoefficient = 0.f;
}

void URifleTearGasAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	//지속 피해 유형이라 비치명, 체력 1까지만 깎이고 이 피해로는 죽지 않는다
	const int32 amount = FMath::Max(1, FMath::RoundToInt(affected->GetMaxHp() * maxHpRatioPercent / 100.f));
	affected->ReceiveDamage(amount, false);
}