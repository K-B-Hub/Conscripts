//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/Stress/SuicidalUrgeAilment.h"
#include "Characters/CharacterBase.h"

//턴 시작 시 입는 고정 피해량
static constexpr int32 SuicidalUrgeDamage = 20;

USuicidalUrgeAilment::USuicidalUrgeAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "SuicidalUrge_Name", "자살충동");
	ailmentDescription = NSLOCTEXT("Ailment", "SuicidalUrge_Description", "스스로를 해치려는 충동에 시달린다. 매 턴 시작 시 20의 고정 피해를 입는다");

	ailmentTurn = 2;
	isStart = true;
	isEnd = false;
	isStackable = false;

	aiDamagePerTurn = SuicidalUrgeDamage;
	aiControlCoefficient = 0.f;
}

void USuicidalUrgeAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	//방어력 무시 고정 피해, 상태이상 피해는 비치명이라 hp 1까지만 깎임
	affected->ReceiveDamage(SuicidalUrgeDamage, false);
}