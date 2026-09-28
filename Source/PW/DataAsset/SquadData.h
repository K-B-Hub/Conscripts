//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SquadData.generated.h"

class AEnemyBase;

//분대원 한 명, 직업과 자리를 같은 원소에 둬서 인원과 슬롯이 어긋날 수 없게 한다
USTRUCT(BlueprintType)
struct FSquadMember
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AEnemyBase> EnemyClass;

	//앵커 로컬 오프셋, X가 앵커 정면
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector2D Offset = FVector2D::ZeroVector;

	//앵커 방향 기준 추가 회전, 측면을 경계하는 배치에 쓴다
	//적의 감지 부채꼴이 이 방향을 따르므로 배치뿐 아니라 시야에도 영향을 준다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float YawOffset = 0.f;
};

//적 한 분대의 구성과 대형, 앵커 위에 그대로 찍힌다
//대형을 앵커가 아니라 이쪽에 두는 이유는 앵커를 맵에 잔뜩 뿌려도 수작업이 늘지 않기 때문
UCLASS()
class PW_API USquadData : public UDataAsset
{
	GENERATED_BODY()

public:
	const TArray<FSquadMember>& GetMembers() const { return members; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Squad")
	TArray<FSquadMember> members;
};