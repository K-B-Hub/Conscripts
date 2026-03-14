// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CursorIndicator.generated.h"

class UDecalComponent;
class UWidgetComponent;
class UMoveIndicatorWidget;
class ACharacterBase;

// 마우스 커서를 따라다니며 이동 목표 지점을 시각화하는 액터.
// - moveDecal: 바닥에 이동 목표 데칼 표시
// - distanceWidget: 활성 캐릭터 → 커서까지 NavMesh 경로 거리(m) 표시
UCLASS()
class PW_API ACursorIndicator : public AActor
{
	GENERATED_BODY()

public:
	ACursorIndicator();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ─── 컴포넌트 ────────────────────────────────────────────

	// 바닥에 표시되는 이동 목표 데칼
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UDecalComponent> moveDecal;

	// 경로 거리를 표시하는 위젯 컴포넌트 (Widget Class: WBP_MoveIndicator)
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UWidgetComponent> distanceWidget;

	// 거리 위젯의 위치 오프셋 (월드 공간 기준, cm)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Visual")
	FVector widgetOffset = FVector(0.f, 0.f, 80.f);

	// ─── 인터페이스 ──────────────────────────────────────────

	// 턴이 시작된 캐릭터를 설정. BattleController에서 호출
	void SetActiveUnit(ACharacterBase* Unit);

private:
	// 현재 이동 명령 대상 캐릭터 (GC 방지를 위해 UPROPERTY 필수)
	UPROPERTY()
	TObjectPtr<ACharacterBase> activeUnit = nullptr;

	// 경로 거리 계산 쓰로틀링 타이머
	float pathUpdateTimer = 0.f;
	static constexpr float pathUpdateInterval = 0.1f;

	// NavMesh 경로 거리를 계산하여 distanceWidget을 갱신
	void UpdatePathDistance();
};
