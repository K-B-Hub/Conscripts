//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "AI/AIAction.h"
#include "TimerManager.h"
#include "UtilityAIComponent.generated.h"

class UActiveSkillBase;
class ACharacterBase;
class UAIPersonalityData;
class UBuffBase;
class UAilmentBase;

//버프가 가감하는 스탯 식별자, StatLeverage 환산용
enum class EAIBuffStat : uint8
{
	Hp, Atk, Def, Speed, Skill, MovingPoint,
	DamageReduction, DamageAmplification, Penetration,
	Accuracy, Evasion, Critical,
};

//적 전투 턴 행동을 결정하는 컴포넌트
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class PW_API UUtilityAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUtilityAIComponent();
	
	virtual void BeginPlay() override;

protected:
	//시전자 관점 pickTeam 대상 수집, EnemyOnly=플레이어, AllyOnly=AI진영, Any=둘다
	void ResolvePickTargets(EPickTeam Team, TArray<ACharacterBase*>& Out) const;

	//레벨 내 살아있는 모든 캐릭터 수집
	void CollectAllCharacters(TArray<ACharacterBase*>& Out) const;

	//범위 오버랩 반경, 인디케이터 규칙과 동일
	static float AreaOverlapRadius(const UActiveSkillBase* Skill);

	//EAreaTarget 팀 필터 통과 여부
	bool PassesAreaTargetFilter(const UActiveSkillBase* Skill, const ACharacterBase* Character) const;

	//aim 지점 중심 범위에 영향받는 대상 수집
	void CollectAreaAffected(const UActiveSkillBase* Skill, const FVector& AimLoc,
	                         TArray<ACharacterBase*>& Out) const;

	//지정 위치에서 aim 지점까지 시야선 확보 여부
	bool HasLOSToLocation(const FVector& FromLoc, const FVector& AimLoc) const;

	//유효 시전 위치 후보 수집, TPair는 위치와 경로 길이
	//bGateAimRange: aim까지의 사거리로 위치를 필터할지, 멀티픽은 픽별 검사에 위임하려 false 사용
	void GatherCastPositions(const FVector& AimLoc, float PickRangeCm, bool bAssumeMoved,
	                         TArray<TPair<FVector, float>>& Out, bool bGateAimRange = true) const;

	//스킬 하나에 대한 모든 후보 열거, selectMode별 분기 (버프/상태이상 스킬 포함)
	void EnumerateSkillActions(UActiveSkillBase* Skill, bool bAssumeMoved,
	                           TArray<FAIAction>& OutCandidates) const;

	//SinglePick 단일 대상 후보 열거
	void EnumerateSingleTarget(UActiveSkillBase* Skill, ACharacterBase* Target, bool bAssumeMoved,
	                           TArray<FAIAction>& OutCandidates) const;

	//SinglePick 멀티픽(pickCount>1) 후보 열거, 픽별 효용(피해/회복/rider)으로 정렬
	void EnumerateMultiPick(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& PickTargets,
	                        bool bAssumeMoved, TArray<FAIAction>& OutCandidates) const;

	//GroundPoint 후보 열거, 최다 적중 지면점 탐색
	void EnumerateGroundPoint(UActiveSkillBase* Skill, bool bAssumeMoved,
	                          TArray<FAIAction>& OutCandidates) const;

	//영향 대상 집합으로 후보 하나 구성, 단일 패스로 피해/회복/rider 가치 동시 집계
	//대표는 Targets[0], 우선순위 = 최대 피해 적군 > 최대 부상 아군 > 최대 rider 가치 대상
	//유효 가치가 전혀 없으면 후보 미생성
	void BuildActionFromCast(UActiveSkillBase* Skill, const FVector& CastFrom, float PathLenCm,
	                         const TArray<ACharacterBase*>& Affected,
	                         TArray<FAIAction>& OutCandidates) const;

	//스킬 rider(버프/상태이상)의 대상 기준 가치, hitP·팀부호·턴수·중복 게이트 반영
	//반환값 = OutBuffValue + OutAilmentValue
	float ComputeRiderValue(const UActiveSkillBase* Skill, const ACharacterBase* Target, float HitP,
	                        float& OutBuffValue, float& OutAilmentValue) const;

	//순수 이동 후보 열거
	void EnumerateMoveActions(TArray<FAIAction>& OutCandidates) const;

	//대기 후보 추가
	void EnumerateWaitAction(TArray<FAIAction>& OutCandidates) const;

	//전체 행동 후보 열거
	void EnumerateActions(bool bExcludeMove, TArray<FAIAction>& OutCandidates) const;

	//디스크 위치 샘플 생성
	static void FibonacciDiskSample(const FVector& Center, float RadiusCm, int32 N,
	                                TArray<FVector>& OutSamples);

	//지정 위치의 예상 피격 피해 계산
	float ComputeIncomingDangerAt(const FVector& AtLocation) const;

	//최근접 생존 플레이어까지의 거리, 없으면 -1
	float DistToNearestPlayer(const FVector& From) const;

	//전장 평균 스탯 스냅샷, 극단성 배율의 분모
	struct FBattlefieldStatAverages
	{
		float hp = 0.f;
		float atk = 0.f;
		float def = 0.f;
		float speed = 0.f;
		float skill = 0.f;
		float movingPoint = 0.f;
		float damageReduction = 0.f;
		float damageAmplification = 0.f;
		float penetration = 0.f;
		float accuracy = 0.f;
		float evasion = 0.f;
		float critical = 0.f;
	};

	//전장 평균 스탯 갱신, 스텝당 1회
	void UpdateBattlefieldAverages();

	//스탯 1이 대상에게 기대 HP 몇 점의 가치인지 환산, 하이브리드(플랫=상수, 퍼센트=앵커×0.01, 파생=합성)
	float StatLeverage(EAIBuffStat Stat, const ACharacterBase* Target) const;

	//버프 CDO의 스탯 델타를 대상 기준 턴당 가치로 환산, 극단성 배율 포함 (팀부호/턴수/명중 미적용)
	float BuffValueForTarget(const UBuffBase* BuffCDO, const ACharacterBase* Target) const;

	//상태이상 CDO를 대상 기준 턴당 가치로 환산, DoT+제어 합산형
	float AilmentValueForTarget(const UAilmentBase* AilmentCDO, const ACharacterBase* Target) const;

	//극단성 배율 1 + w×|e|, e=대상 스탯/전장 평균−1, w는 (팀, e부호)별 성향 선호
	float ExtremenessMultiplier(float TargetStat, float BattlefieldAvg, bool bSameTeam) const;

	//대상의 턴당 기대 가해 피해, 위협 프로파일 기반 (E_out 앵커)
	float ComputeExpectedOutput(const ACharacterBase* Target) const;

	//대상이 상대 진영에게서 받는 기대 피해 합 (E_in 앵커), OutRawDamage는 명중률 미적용 원시 합
	//전략적 추정이라 LOS 생략, 사거리 게이트만 적용
	float ComputeIncomingFor(const ACharacterBase* Target, float& OutRawDamage) const;

	//대상의 상대 진영 평균 def, penetration 레버리지용
	float OpposingTeamAverageDef(const ACharacterBase* Target) const;

	//전장 평균 스탯 캐시
	FBattlefieldStatAverages battlefieldAvg;

	//공격 위치 샘플 수
	UPROPERTY(EditDefaultsOnly, Category = "AI|Sampling")
	int32 attackPositionSamples = 10;

	//이동 링당 방향 수
	UPROPERTY(EditDefaultsOnly, Category = "AI|Sampling")
	int32 moveDirectionsPerRing = 10;

	//소유 캐릭터 캐시
	UPROPERTY(Transient)
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

public:
	//적 턴 행동 시작
	void ExecuteTurn();

	//턴 종료 신호
	DECLARE_MULTICAST_DELEGATE(FOnAITurnComplete);
	FOnAITurnComplete OnTurnComplete;

protected:
	//턴당 최대 행동 수
	UPROPERTY(EditDefaultsOnly, Category = "AI|Loop")
	int32 maxActionsPerTurn = 8;

	//행동 사이 대기 시간
	UPROPERTY(EditDefaultsOnly, Category = "AI|Loop")
	float actionDelay = 0.4f;

	//최고점 후보 선택
	const FAIAction* ScoreAndPickBest(const TArray<FAIAction>& Candidates, TArray<float>* OutScores = nullptr) const;

	//후보 디버그 시각화
	void DrawDebugCandidates(const TArray<FAIAction>& Candidates,
	                         const TArray<float>& Scores,
	                         const FAIAction* Best) const;

	//단일 후보 점수 계산
	float ScoreAction(const FAIAction& Action) const;

	//적별 성향 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|Personality")
	TObjectPtr<UAIPersonalityData> personalityData;

	//다음 행동 단계 실행
	void StepNext();

	//턴 종료 처리
	void FinishTurn();

	//제자리 공격 실행
	void ExecuteAttackImmediate(const FAIAction& Action);

	//AI 이동 시작
	void StartMoveTo(const FVector& Dest, float PathLengthCm);

	//AI 이동 완료 콜백
	UFUNCTION()
	void OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);

	//행동 후 종료 여부 검사
	bool ShouldStopAfterStep() const;

	//콜백 체인 상태
	int32 currentIter = 0;
	bool lastWasMove = false;
	int32 apBefore = 0;
	float moveBefore = 0.f;
	bool bHasPendingAfterMove = false;
	FAIAction pendingActionAfterMove;

	//행동 대기 타이머
	FTimerHandle stepTimerHandle;
};
