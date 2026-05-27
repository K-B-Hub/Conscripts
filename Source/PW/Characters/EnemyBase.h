// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "EnemyBase.generated.h"

class AAllyCharacterBase;
class AEnemyAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEnemyDeath, AEnemyBase*, DeadEnemy, AAllyCharacterBase*, Killer);

UCLASS()
class PW_API AEnemyBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AEnemyBase();

	// 사망 시 경험치 분배용 델리게이트
	FOnEnemyDeath OnEnemyDeath;

	virtual void HandleDeath() override;
	virtual void Tick(float DeltaTime) override;

	// 적 턴 시작 — Super 호출 후 본인 AIController에 통지하여 BT 진행 결정 위임
	virtual void InitTurn() override;

	// 비전투 상태에서 전방 감지에 사용하는 부채꼴 — AIController가 폴링하여 전투 전이 판단
	// 시야 차단(엄폐/벽) 검사는 본 클래스에서 다루지 않음 — AIController 책임
	bool IsInDetectionFan(const FVector& worldPoint) const;

	float GetDetectionRadius() const { return detectionRadius; }
	float GetDetectionAngle() const { return detectionAngle; }

protected:
	// 부채꼴 반경 (cm)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection")
	float detectionRadius = 1200.f;

	// 부채꼴 전체 각도 (degrees) — 중심축(전방) 기준 좌우 각각 detectionAngle/2
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection", meta = (ClampMin = "0.0", ClampMax = "360.0"))
	float detectionAngle = 120.f;

	// 에디터에서 부채꼴 디버그 표시
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Detection")
	bool bShowDetectionDebug = false;
};
