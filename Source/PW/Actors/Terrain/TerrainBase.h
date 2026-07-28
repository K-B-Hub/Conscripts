//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TerrainBase.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UNiagaraComponent;
class ACharacterBase;
class UBuffBase;
class UAilmentBase;

//모든 지형의 기반 액터, 파생 클래스에서 필요한 훅만 override
//훅은 전부 빈 구현이므로 쓰지 않는 지형은 아무 비용도 치르지 않음
UCLASS(Abstract, Blueprintable)
class PW_API ATerrainBase : public AActor
{
	GENERATED_BODY()

public:
	ATerrainBase();

	//지형 이름 (UI 표시용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Identity")
	FText terrainName;
	//지형 설명 (UI 툴팁용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Identity")
	FText terrainDescription;

	//체류 중 적용되는 스탯 델타, 진입 시 +1 / 이탈 시 -1 대칭 가감
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 hp = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 atk = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 speed = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 skill = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 def = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	float movingPoint = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 mentality = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 actionPoint = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 damageReduction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 damageAmplification = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	int32 penetration = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat")
	float sight = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat|Combat")
	float accuracy = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat|Combat")
	float evasion = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Stat|Combat")
	float critical = 0.0;

	//진입 시 부여할 버프, 지형 이탈과 무관하게 버프 자체 턴수로 만료
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Buff")
	TArray<TSubclassOf<UBuffBase>> enterBuffs;
	//진입 시 부여할 상태이상, 지형 이탈과 무관하게 상태이상 자체 턴수로 만료
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Ailment")
	TArray<TSubclassOf<UAilmentBase>> enterAilments;

	//지형 내 이동력 소모 배율, 2.0이면 같은 거리에 두 배 소모
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|Move")
	float movingPointCostMultiplier = 1.f;

	//AI 평가용 체류 가치(턴당, 기대 HP 환산), 음수면 회피 대상
	//지형은 객관적 손익만 기술하고, 자기 상태에 비춘 해석은 AI가 담당
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|AI")
	float aiBaseValue = 0.f;

	//AI 평가용 통과 손익(1회, 기대 HP 환산), 진입만으로 효과가 발동하는 지형만 지정
	//턴 훅으로만 동작하는 지형은 통과해도 무해하므로 0
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Terrain|AI")
	float aiPassThroughValue = 0.f;

	//진입 즉시 1회, 오버렙 기반 지형이 사용
	virtual void OnCharacterEnter(ACharacterBase* character) {}
	//이탈 즉시 1회, 오버렙 기반 지형이 사용
	virtual void OnCharacterExit(ACharacterBase* character) {}
	//체류 중 턴 시작 시
	virtual void OnCharacterTurnStart(ACharacterBase* character) {}
	//체류 중 턴 종료 시
	virtual void OnCharacterTurnEnd(ACharacterBase* character) {}

	//임의 위치가 지형 내부인지, AI가 이동 후보를 사전 평가할 때 사용
	bool IsLocationInside(const FVector& location) const;

	//현재 체류 중인 캐릭터 목록, 주변 대상 판정이 필요한 지형이 사용
	const TArray<TWeakObjectPtr<ACharacterBase>>& GetOccupants() const { return occupants; }

	UBoxComponent* GetTerrainBox() const { return terrainBox; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//오버렙 판정용 박스, 루트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain|Component")
	TObjectPtr<UBoxComponent> terrainBox;

	//지형 시각 표현
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain|Component")
	TObjectPtr<UStaticMeshComponent> terrainMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Terrain|Component")
	TObjectPtr<UNiagaraComponent> terrainVfx;

	//체류 중인 캐릭터 목록
	TArray<TWeakObjectPtr<ACharacterBase>> occupants;

	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};