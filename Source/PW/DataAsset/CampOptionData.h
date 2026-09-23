//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CampOptionData.generated.h"

class ACampGameMode;

//야영지 정비 선택지 하나
//회복·휴식·증원·보급이 모두 "비율 회복 + 충원 인원" 한 가지 모양이라 파생 없이 인스턴스로 구분한다
//데이터 모양이 다른 선택지가 생기면 그때 파생으로 나눈다
UCLASS(BlueprintType)
class PW_API UCampOptionData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	FText Title;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp")
	FText Description;

	//최대 체력 대비 회복 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp|Restore", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float hpRatio = 0.f;

	//최대 스트레스 대비 감소 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp|Restore", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float stressRatio = 0.f;

	//최대 전투 자원 대비 회복 비율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp|Restore", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float battleResourceRatio = 0.f;

	//새로 합류시킬 동료 수, 0이면 충원 없음
	//0보다 크면 직업 선택과 강화 선택이 이어지므로 선택 즉시 야영지를 떠나지 않는다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camp|Recruit", meta = (ClampMin = "0"))
	int32 recruitCount = 0;

	//비율 회복을 야영지 전원에게 적용한다, 충원은 컨트롤러가 이어서 진행
	void ApplyRestoration(ACampGameMode* camp) const;
};