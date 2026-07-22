//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestAfterSlay.h"
#include "Characters/EnemyBase.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"

UTestAfterSlay::UTestAfterSlay()
{
	skillName = NSLOCTEXT("Skill", "TestAfterSlay_Name", "테스트");
	skillDescription = NSLOCTEXT("Skill", "TestAfterSlay_Description", "123123");
	
	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::AfterSlay;
	conditionType = EConditionalType::None;
}

void UTestAfterSlay::Execute_AfterSlay(FVector slainLocation)
{
	UE_LOG(LogTemp, Log, TEXT("[TestAfterSlay] Execute 호출 — slainLocation=%s"), *slainLocation.ToString());

	UWorld* world = GetWorld();
	if (!world) return;

	constexpr float Radius = 500.f;

	TArray<FOverlapResult> overlaps;
	world->OverlapMultiByObjectType(
		overlaps,
		slainLocation,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECC_Pawn),
		FCollisionShape::MakeSphere(Radius)
	);

	//같은 액터에 Pawn 채널 컴포넌트가 여러 개일 수 있으므로 예외처리
	TSet<AEnemyBase*> hitOnce;
	for (const FOverlapResult& result : overlaps)
	{
		if (AEnemyBase* enemy = Cast<AEnemyBase>(result.GetActor()))
		{
			if (hitOnce.Contains(enemy)) continue;
			hitOnce.Add(enemy);
			enemy->ReceiveDamage(1, false);
		}
	}
}
