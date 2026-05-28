// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UtilityAIComponent.h"
#include "AI/AINavigationHelper.h"
#include "AI/AIPersonalityData.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "GameMode/BattleGameMode.h"
#include "Engine/World.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"

UUtilityAIComponent::UUtilityAIComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UUtilityAIComponent::BeginPlay()
{
	Super::BeginPlay();
	ownerCharacter = Cast<ACharacterBase>(GetOwner());
}

void UUtilityAIComponent::FibonacciDiskSample(const FVector& Center, float RadiusCm, int32 N,
                                              TArray<FVector>& OutSamples)
{
	OutSamples.Reset(N);
	if (N <= 0 || RadiusCm <= 0.f) return;

	// 황금각 = π * (3 - √5) — 디스크 균등 샘플의 표준 각도
	const float goldenAngle = PI * (3.f - FMath::Sqrt(5.f));
	for (int32 i = 0; i < N; ++i)
	{
		// r ∝ √t (디스크 균등성), t ∈ (0, 1]
		const float t = (i + 0.5f) / static_cast<float>(N);
		const float r = RadiusCm * FMath::Sqrt(t);
		const float theta = i * goldenAngle;
		OutSamples.Emplace(
			Center.X + r * FMath::Cos(theta),
			Center.Y + r * FMath::Sin(theta),
			Center.Z
		);
	}
}

void UUtilityAIComponent::EnumerateAttackActionsForTarget(UActiveSkillBase* Skill, ACharacterBase* Target,
                                                          bool bAssumeMoved,
                                                          TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !Target || !ownerCharacter) return;
	if (Target->IsDead()) return;

	const FVector targetLoc = Target->GetActorLocation();
	const FVector casterLoc = ownerCharacter->GetActorLocation();
	const float pickRangeCm = Skill->pickRange;   // ActiveSkillBase.h:58 — cm, 0=무제한

	// 위치 후보 — 옵션 B에 따라 두 그룹 명확 분리
	TArray<FVector> samples;
	if (bAssumeMoved)
	{
		// 이동 후 후보: 디스크 샘플만. pickRange=0이면 의미 없으므로 생략.
		if (pickRangeCm > 0.f)
		{
			FibonacciDiskSample(targetLoc, pickRangeCm, attackPositionSamples, samples);
		}
	}
	else
	{
		// 제자리 후보: 캐스터 위치 하나만
		samples.Add(casterLoc);
	}

	const float casterBudgetCm = ownerCharacter->GetCurrentMovingPoint() * 100.f;

	for (const FVector& sample : samples)
	{
		// 사전 컷1 — 캐스터→샘플 직선거리 > 잔여 이동력이면 도달 불가 (제자리는 거리 0이라 자동 통과)
		const float distToSampleCm = FVector::Dist(casterLoc, sample);
		if (distToSampleCm > casterBudgetCm) continue;

		// 사전 컷2 — 샘플 → 대상 거리가 사거리 초과면 시전 불가
		if (pickRangeCm > 0.f && FVector::Dist(sample, targetLoc) > pickRangeCm) continue;

		float pathLenCm = 0.f;
		if (!UAINavigationHelper::CanReach(ownerCharacter, sample, pathLenCm)) continue;

		// 사선 검사 — sample 위치에서 Target까지 가시. 벽 너머/엄폐 뒤 적은 시전 불가.
		// 버프/지원은 같은 팀이라도 사선 필요. 만약 사선 없이 시전 가능한 스킬 유형이 생기면 여기 분기 추가.
		if (!UAINavigationHelper::HasLineOfSightFrom(ownerCharacter, sample, Target)) continue;

		FAIAction action;
		action.Skill = Skill;
		action.Target = Target;
		action.CastFrom = sample;
		action.PathLengthCm = pathLenCm;
		action.Preview = Target->PreviewDamage(Skill, ownerCharacter, sample);
		// SkillType이 Buff면 Support로 분류 — 같은 시전 메커니즘이지만 스코어링이 다름
		action.Type = (Skill->skillType == ESkillType::Buff) ? EAIActionType::Support : EAIActionType::Attack;

		OutCandidates.Add(action);
	}
}

void UUtilityAIComponent::EnumerateMoveActions(const TArray<ACharacterBase*>& Targets,
                                               TArray<FAIAction>& OutCandidates) const
{
	if (!ownerCharacter || Targets.Num() == 0) return;
	const float casterBudgetCm = ownerCharacter->GetCurrentMovingPoint() * 100.f;
	if (casterBudgetCm <= 0.f) return;

	const FVector casterLoc = ownerCharacter->GetActorLocation();

	// 가장 가까운 적 한 명 — 그쪽 방향으로 잔여 이동력만큼 전진
	ACharacterBase* nearest = nullptr;
	float bestDistSq = TNumericLimits<float>::Max();
	for (ACharacterBase* t : Targets)
	{
		if (!IsValid(t) || t->IsDead()) continue;
		const float dSq = FVector::DistSquared(casterLoc, t->GetActorLocation());
		if (dSq < bestDistSq) { bestDistSq = dSq; nearest = t; }
	}
	if (!nearest) return;

	const FVector toTarget = nearest->GetActorLocation() - casterLoc;
	const float distCm = toTarget.Size();
	if (distCm <= KINDA_SMALL_NUMBER) return;

	const float advanceCm = FMath::Min(distCm, casterBudgetCm);
	const FVector advancePoint = casterLoc + (toTarget / distCm) * advanceCm;

	float pathLenCm = 0.f;
	if (!UAINavigationHelper::CanReach(ownerCharacter, advancePoint, pathLenCm)) return;

	FAIAction action;
	action.CastFrom = advancePoint;
	action.PathLengthCm = pathLenCm;
	action.Type = EAIActionType::Move;
	OutCandidates.Add(action);
}

void UUtilityAIComponent::EnumerateActions(bool bExcludeMove, TArray<FAIAction>& OutCandidates) const
{
	OutCandidates.Reset();
	if (!ownerCharacter) return;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) { EnumerateWaitAction(OutCandidates); return; }

	// 공격 대상 = 살아있는 아군 (적 AI 시점). 지원 대상(=다른 적)은 §5 정교화 시 추가.
	TArray<ACharacterBase*> targets;
	for (AAllyCharacterBase* a : gm->GetAllies())
	{
		if (IsValid(a) && !a->IsDead()) targets.Add(a);
	}

	USkillComponent* sc = ownerCharacter->GetSkillComponent();
	TArray<UActiveSkillBase*> skills = sc ? sc->GetActiveSkills() : TArray<UActiveSkillBase*>();

	// 1) 제자리 공격 후보 — 현재 isMoved 상태에서 평가
	for (UActiveSkillBase* skill : skills)
	{
		if (!skill || !skill->CanExecute()) continue;
		for (ACharacterBase* t : targets)
		{
			EnumerateAttackActionsForTarget(skill, t, /*bAssumeMoved=*/false, OutCandidates);
		}
	}

	// 2) 이동 후 공격 + 순수 이동 — 직전 행동이 이동이 아니고 이동력 남아있을 때만
	const bool bCanMoveMore = !bExcludeMove && (ownerCharacter->GetCurrentMovingPoint() > 0.f);
	if (bCanMoveMore)
	{
		// 옵션 B 가상 토글 — 이동 후 BeforeMove 보너스가 calc*에 반영된 상태로 그룹 평가
		// 이미 isMoved=true면(이번 턴에 이미 이동했음) 토글/rollback 불필요
		const bool prevIsMoved = ownerCharacter->IsMoved();
		if (!prevIsMoved)
		{
			ownerCharacter->OnMoveStateChanged(true);
		}

		for (UActiveSkillBase* skill : skills)
		{
			if (!skill || !skill->CanExecute()) continue;
			for (ACharacterBase* t : targets)
			{
				EnumerateAttackActionsForTarget(skill, t, /*bAssumeMoved=*/true, OutCandidates);
			}
		}

		EnumerateMoveActions(targets, OutCandidates);

		if (!prevIsMoved)
		{
			ownerCharacter->OnMoveStateChanged(false);   // rollback
		}
	}

	// 3) 대기 후보 — 항상 명시적 후보로 포함
	EnumerateWaitAction(OutCandidates);
}

const FAIAction* UUtilityAIComponent::ScoreAndPickBest(const TArray<FAIAction>& Candidates) const
{
	if (Candidates.Num() == 0) return nullptr;

	const FAIAction* bestPtr = nullptr;
	float bestScore = -TNumericLimits<float>::Max();

	for (const FAIAction& c : Candidates)
	{
		const float s = ScoreAction(c);
		if (s > bestScore)
		{
			bestScore = s;
			bestPtr = &c;
		}
	}
	return bestPtr;
}

float UUtilityAIComponent::ScoreAction(const FAIAction& Action) const
{
	// personality null guard — CDO의 디폴트 가중치(Normal 성향)로 fallback
	const UAIPersonalityData* p = personalityData
		? personalityData.Get()
		: GetDefault<UAIPersonalityData>();

	float score = 0.f;

	if (Action.IsWait())
	{
		// 대기는 절대 점수만 — 모든 다른 후보가 wait 점수 미만일 때만 선택됨
		score = p->weightWait;
	}
	else if (Action.IsMove())
	{
		// 순수 이동 — 접근 보너스 vs 이동 거리 페널티. 공격적/수비적 차이가 여기서 표현됨.
		score += Action.PathLengthCm * p->weightMoveAdvance;
		score += Action.PathLengthCm * p->weightDistancePenalty;
	}
	else
	{
		// Attack / Support — 위협도 축
		const float hitP  = Action.Preview.HitChance / 100.f;
		const float critP = Action.Preview.CritChance / 100.f;

		// 기대 피해 = HitChance × (비치명확률 × NormalDamage + 치명확률 × CritDamage)
		const float expected = hitP * ((1.f - critP) * Action.Preview.NormalDamage
		                              + critP * Action.Preview.CritDamage);
		score += expected * p->weightExpectedDamage;

		// 처치 보너스 — 두 단계 분리 가중 (bCanKill·bCanCritKill 상호배타)
		if (Action.Preview.bCanKill)
		{
			score += p->weightCanKillBonus * hitP;
		}
		if (Action.Preview.bCanCritKill)
		{
			score += p->weightCanCritKillBonus * hitP * critP;
		}

		// 이동 거리 페널티 — 이동 후 공격이면 양수 PathLengthCm
		score += Action.PathLengthCm * p->weightDistancePenalty;
	}

	// 노이즈 — 결정론적이지 않은 성향("랜덤") 표현
	if (p->noiseTemperature > 0.f)
	{
		score += FMath::FRandRange(-p->noiseTemperature, p->noiseTemperature);
	}

	return score;
}

void UUtilityAIComponent::ExecuteTurn()
{
	if (!ownerCharacter) { FinishTurn(); return; }

	// 상태 초기화 — 매 턴 시작 시 콜백 체인 상태 리셋
	currentIter = 0;
	lastWasMove = false;
	bHasPendingAfterMove = false;

	StepNext();
}

void UUtilityAIComponent::StepNext()
{
	if (!ownerCharacter) { FinishTurn(); return; }

	// 방어선 ③ — hard cap. 정상 동작에선 안 걸려야 함.
	if (currentIter >= maxActionsPerTurn)
	{
		UE_LOG(LogTemp, Warning, TEXT("[UtilityAI] %s hit MaxActionsPerTurn(%d) — possible loop bug"),
			*GetNameSafe(ownerCharacter), maxActionsPerTurn);
		FinishTurn();
		return;
	}

	apBefore = ownerCharacter->GetCurrentActionPoint();
	moveBefore = ownerCharacter->GetCurrentMovingPoint();

	TArray<FAIAction> candidates;
	EnumerateActions(/*bExcludeMove=*/lastWasMove, candidates);
	if (candidates.Num() == 0) { FinishTurn(); return; }

	const FAIAction* best = ScoreAndPickBest(candidates);
	if (!best || best->IsWait()) { FinishTurn(); return; }

	++currentIter;

	// 행동 디스패치 — 세 케이스
	if (best->IsMove())
	{
		// 순수 이동 — 다음 step에서 이동 후보 제외 (방어선 ②)
		lastWasMove = true;
		bHasPendingAfterMove = false;
		StartMoveTo(best->CastFrom, best->PathLengthCm);
		return;
	}

	if (best->PathLengthCm > 0.f)
	{
		// 이동 후 공격 — 본 행동은 공격이라 lastWasMove=false (이동-스킬-이동 허용)
		lastWasMove = false;
		pendingActionAfterMove = *best;
		bHasPendingAfterMove = true;
		StartMoveTo(best->CastFrom, best->PathLengthCm);
		return;
	}

	// 제자리 공격 — 즉시 실행 후 다음 step
	lastWasMove = false;
	ExecuteAttackImmediate(*best);
	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	StepNext();
}

void UUtilityAIComponent::ExecuteAttackImmediate(const FAIAction& Action)
{
	if (!Action.Skill || !Action.Target || !ownerCharacter) return;

	// 대상 방향 회전 (Self/GroundPoint 제외)
	if (Action.Skill->selectMode != ESelectMode::Self)
	{
		FVector dir = Action.Target->GetActorLocation() - ownerCharacter->GetActorLocation();
		dir.Z = 0.f;
		if (!dir.IsNearlyZero())
		{
			ownerCharacter->SetActorRotation(dir.Rotation());
		}
	}

	if (USkillComponent* sc = ownerCharacter->GetSkillComponent())
	{
		sc->DirectExecute(Action.Skill, Action.Target);
	}
}

void UUtilityAIComponent::StartMoveTo(const FVector& Dest, float PathLengthCm)
{
	if (!ownerCharacter) { FinishTurn(); return; }

	AAIController* aic = Cast<AAIController>(ownerCharacter->GetController());
	if (!aic) { FinishTurn(); return; }

	// 이동 시작 — BeforeMove 패시브 전이 (이미 true면 no-op)
	ownerCharacter->OnMoveStateChanged(true);

	// 이동력 사전 차감 — PathLengthCm(cm) → m
	ownerCharacter->ConsumeMovingPoint(PathLengthCm / 100.f);

	// 중복 바인딩 방지 — 매 호출마다 unbind 후 add
	aic->ReceiveMoveCompleted.RemoveDynamic(this, &UUtilityAIComponent::OnAIMoveCompleted);
	aic->ReceiveMoveCompleted.AddUniqueDynamic(this, &UUtilityAIComponent::OnAIMoveCompleted);

	FAIMoveRequest req(Dest);
	req.SetUsePathfinding(true);
	req.SetAcceptanceRadius(10.f);

	const EPathFollowingRequestResult::Type r = aic->MoveTo(req);
	if (r == EPathFollowingRequestResult::Failed || r == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		// 즉시 완료/실패 — 콜백이 안 올 수 있어 수동 진행
		OnAIMoveCompleted(FAIRequestID::InvalidRequest, EPathFollowingResult::Success);
	}
}

void UUtilityAIComponent::OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	// MoveComplete 패시브 — 후속 공격에 영향 줄 수 있도록 시전 전에 디스패치
	if (UWorld* world = GetWorld())
	{
		if (ABattleGameMode* gm = world->GetAuthGameMode<ABattleGameMode>())
		{
			gm->BroadcastMoveComplete();
		}
	}

	// 이동 후 시전이 예약돼 있으면 처리
	if (bHasPendingAfterMove)
	{
		bHasPendingAfterMove = false;
		ExecuteAttackImmediate(pendingActionAfterMove);
	}

	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	StepNext();
}

bool UUtilityAIComponent::ShouldStopAfterStep() const
{
	if (!ownerCharacter) return true;
	const int32 ap = ownerCharacter->GetCurrentActionPoint();
	const float mp = ownerCharacter->GetCurrentMovingPoint();
	// 방어선 ① — 자원 미감소 (스텁/버그 안전망)
	if (ap == apBefore && FMath::IsNearlyEqual(mp, moveBefore)) return true;
	// 자원 완전 소진
	if (ap <= 0 && mp <= 0.f) return true;
	return false;
}

void UUtilityAIComponent::FinishTurn()
{
	// 콜백 해제 — 다음 턴/다른 적의 콜백 누수 방지
	if (ownerCharacter)
	{
		if (AAIController* aic = Cast<AAIController>(ownerCharacter->GetController()))
		{
			aic->ReceiveMoveCompleted.RemoveDynamic(this, &UUtilityAIComponent::OnAIMoveCompleted);
		}
	}
	OnTurnComplete.Broadcast();
}

void UUtilityAIComponent::EnumerateWaitAction(TArray<FAIAction>& OutCandidates) const
{
	if (!ownerCharacter) return;

	FAIAction action;
	action.Type = EAIActionType::Wait;
	action.CastFrom = ownerCharacter->GetActorLocation();
	OutCandidates.Add(action);
}