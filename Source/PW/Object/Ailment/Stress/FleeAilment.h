//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "FleeAilment.generated.h"

class AAllyCharacterBase;

//스트레스 부정 이벤트 '공황', 2턴간 조작 불가·적에게서 멀어지는 이동 후 턴 종료
UCLASS()
class PW_API UFleeAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	UFleeAilment();
	virtual void Execute(ACharacterBase* affected) override;

private:
	//도주 목적지 선정, 갈 곳이 없으면 false 반환해 이동 없이 턴 종료
	bool PickFleeDestination(AAllyCharacterBase* ally, FVector& outDestination) const;

	//이동 자연 종료(도착·이동력 소진) 시 턴 종료 처리
	void OnFleeMoveCompleted();

	//완료 콜백에서 턴 종료 대상을 복원하기 위한 약참조
	TWeakObjectPtr<AAllyCharacterBase> fleeingAlly;
};
