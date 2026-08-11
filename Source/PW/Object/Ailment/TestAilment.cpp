//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Ailment/TestAilment.h"
#include "Characters/CharacterBase.h"

UTestAilment::UTestAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "Test_Name", "테스트 상태이상");
	ailmentDescription = NSLOCTEXT("Ailment", "Test_Description", "테스트용 상태이상. 매 턴 시작 시 방어력을 무시하고 2의 고정 피해를 입는다");

	ailmentTurn = 3;
	isStart = true;
	isEnd = false;
	isStackable = false;

	aiDamagePerTurn = 2.f;
	aiControlCoefficient = 0.f;
}

void UTestAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	if (!affected) return;

	//방어력 무시 고정 피해
	affected->ReceiveDamage(2, false);
}
