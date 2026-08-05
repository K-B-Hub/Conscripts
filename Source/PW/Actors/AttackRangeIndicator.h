// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackRangeIndicator.generated.h"

class UDecalComponent;
class USphereComponent;
class ACharacterBase;
class AAllyCharacterBase;
class UActiveSkillBase;
class ACursorIndicator;
class USplineMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UWidgetComponent;

//마우스를 따라다니며 스킬 영향 범위를 표시하는 액터
UCLASS()
class PW_API AAttackRangeIndicator : public AActor
{
	GENERATED_BODY()

public:
	AAttackRangeIndicator();

	//Skill 객체로 인디케이터 초기화
	void InitIndicator(ACharacterBase* Caster, UActiveSkillBase* Skill);

	//현재 오버랩 중인 대상 목록 반환
	const TArray<ACharacterBase*>& GetOverlappingTargets() const { return overlappingTargets; }

	//SinglePick 모드에서 현재 스냅된 대상 반환
	ACharacterBase* GetSnappedTarget() const { return snappedTarget.Get(); }

	//현재 위치가 사거리 밖인지 계산
	bool ComputeOutOfRange() const;

	//자동이동용 NavMesh 경로 반환
	const TArray<FVector>& GetMovePath() const;

	//시작→끝 포물선 궤적 폴리라인 생성, 정점은 중간 위 ApexHeight (점프·투척 공용, 무상태)
	static TArray<FVector> BuildArcPath(const FVector& From, const FVector& To, float ApexHeight, int32 Samples);

	//조준 중 계산해둔 포물선 궤적 반환, 실행 시 캐릭터 이동·투척물에 사용
	const TArray<FVector>& GetArcPath() const { return arcPath; }

	//자동이동 중 위치 고정
	void LockAtCurrentPosition();
	//위치 고정 해제
	void Unlock();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	//아치(점프·투척) 궤적 세그먼트 메쉬
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Arc")
	TObjectPtr<UStaticMesh> arcSegmentMesh;
	//도달 가능 궤적 재질
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Arc")
	TObjectPtr<UMaterialInterface> arcMaterialReachable;
	//도달 불가 궤적 재질
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Arc")
	TObjectPtr<UMaterialInterface> arcMaterialUnreachable;
	//궤적 메쉬 단면 스케일
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Arc")
	FVector2D arcMeshScale = FVector2D(1.f, 0.05f);
	//비용 위젯 위치 오프셋
	UPROPERTY(EditDefaultsOnly, Category = "Visual|Arc")
	FVector arcWidgetOffset = FVector(0.f, 0.f, 100.f);
	
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> rootScene;

	//범위 공격 데칼 (단일 대상일 때 비활성)
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> areaDecal;

	//오버랩 감지용 구체 콜리전
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> overlapSphere;

	//아치 궤적 메쉬 부모, 절대 월드 좌표 고정
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> arcMeshRoot;

	//아치 이동력 비용 표시 위젯
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> arcCostWidget;

	//범위 공격 데칼 머티리얼
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	TObjectPtr<UMaterialInterface> areaDecalMaterial;

	//데칼 투영 깊이
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float decalProjectionDepth = 500.f;

	//런타임 생성 아치 궤적 세그먼트 메쉬 풀
	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> arcMeshes;

	//시전자
	TWeakObjectPtr<ACharacterBase> caster;
	float pickRange = 0.f;

	//범위 공격 여부
	bool bIsAreaAttack = false;

	//시전자 위치에 고정할지 여부
	bool bFixedAtCaster = false;

	//사용 중인 스킬 참조
	UPROPERTY()
	TObjectPtr<UActiveSkillBase> cachedSkill;

	//오버랩 중인 캐릭터 목록
	TArray<ACharacterBase*> overlappingTargets;

	//SinglePick 모드에서 현재 스냅된 캐릭터
	TWeakObjectPtr<ACharacterBase> snappedTarget;

	//해당 캐릭터가 범위 스킬 적용 대상인지 확인
	bool IsAreaTarget(ACharacterBase* Character) const;

	//해당 캐릭터가 선택 대상인지 확인
	bool IsPickTarget(ACharacterBase* Character) const;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	//단일 타겟에 대한 데미지 미리 계산
	void RecalculatePendingForTarget(ACharacterBase* Target);

	//CursorIndicator 클래스 (BP에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "Path")
	TSubclassOf<ACursorIndicator> cursorIndicatorClass;

	//사거리 밖일 때 스폰된 CursorIndicator 인스턴스
	UPROPERTY()
	TObjectPtr<ACursorIndicator> movePathIndicator;

	//사거리 밖 여부 및 이동 목표 지점
	bool bIsOutOfRange = false;
	FVector moveToPoint = FVector::ZeroVector;

	//조준 중 계산된 포물선 궤적 폴리라인 (arcApexRatio > 0 스킬)
	TArray<FVector> arcPath;

	//포물선 궤적 샘플 개수
	UPROPERTY(EditDefaultsOnly, Category = "Arc")
	int32 arcSampleCount = 24;

	//직전 프레임의 자동이동 필요 여부
	bool bPrevAutoMoveNeeded = false;

	//자동이동 중 위치 잠금
	bool bIsLocked = false;

	//NavMesh 경로 위에서 사거리 내 최적 이동 지점 계산
	void ComputeOptimalMovePoint();

	//아치 궤적 폴리라인(arcPath)을 풀링 스플라인 메쉬로 렌더, 도달 여부로 재질 선택
	void RebuildArcMeshes(bool bReachable);

	//최적 이동 지점 계산 쓰로틀링 타이머
	float optimalPointUpdateTimer = 0.f;

	//최적 이동 지점 갱신 간격
	UPROPERTY(EditDefaultsOnly, Category = "Path")
	float optimalPointUpdateInterval = 0.05f;

	//CursorIndicator 스폰
	void SpawnMovePathIndicator();
	//CursorIndicator 제거
	void DestroyMovePathIndicator();
};
