//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UtilityAIComponent.h"
#include "AI/AINavigationHelper.h"
#include "AI/AIPersonalityData.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "GameMode/BattleGameMode.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "DrawDebugHelpers.h"

//후보 점수 디버그 시각화 토글
static TAutoConsoleVariable<int32> CVarUtilityAIDebug(
	TEXT("ai.UtilityDebug"),
	0,
	TEXT("Utility AI 후보 점수 시각화. 0=off, 1=on"),
	ECVF_Default);

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

	//디스크 균등 샘플 각도
	const float goldenAngle = PI * (3.f - FMath::Sqrt(5.f));
	for (int32 i = 0; i < N; ++i)
	{
		//디스크 면적 균등 분포
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
	const float pickRangeCm = Skill->pickRange;   //cm, 0이면 무제한

	//이동 가정 여부로 위치 후보 분리
	TArray<FVector> samples;
	if (bAssumeMoved)
	{
		//이동 후 후보는 대상 주변 샘플 사용
		if (pickRangeCm > 0.f)
		{
			FibonacciDiskSample(targetLoc, pickRangeCm, attackPositionSamples, samples);
		}
	}
	else
	{
		//제자리 후보는 현재 위치만 사용
		samples.Add(casterLoc);
	}

	const float casterBudgetCm = ownerCharacter->GetCurrentMovingPoint() * 100.f;

	for (const FVector& sample : samples)
	{
		//잔여 이동력 초과 후보 제외
		const float distToSampleCm = FVector::Dist(casterLoc, sample);
		if (distToSampleCm > casterBudgetCm) continue;

		//사거리 밖 후보 제외
		if (pickRangeCm > 0.f && FVector::Dist(sample, targetLoc) > pickRangeCm) continue;

		float pathLenCm = 0.f;
		if (!UAINavigationHelper::CanReach(ownerCharacter, sample, pathLenCm)) continue;

		//시야선 없는 후보 제외
		if (!UAINavigationHelper::HasLineOfSightFrom(ownerCharacter, sample, Target)) continue;

		FAIAction action;
		action.Skill = Skill;
		action.Target = Target;
		action.CastFrom = sample;
		action.PathLengthCm = pathLenCm;
		action.Preview = Target->PreviewDamage(Skill, ownerCharacter, sample);
		action.IncomingDangerExpected = ComputeIncomingDangerAt(sample);
		//버프는 지원 행동으로 분류
		action.Type = (Skill->skillType == ESkillType::Buff) ? EAIActionType::Support : EAIActionType::Attack;

		OutCandidates.Add(action);
	}
}

void UUtilityAIComponent::EnumerateMoveActions(TArray<FAIAction>& OutCandidates) const
{
	if (!ownerCharacter) return;
	const float casterBudgetCm = ownerCharacter->GetCurrentMovingPoint() * 100.f;
	if (casterBudgetCm <= 0.f) return;

	const FVector casterLoc = ownerCharacter->GetActorLocation();

	//잔여 이동력 비율별 링 샘플
	static const float radiusRatios[] = { 0.1f, 0.3f, 0.5f, 0.7f, 1.0f };
	const int32 numDirs = FMath::Max(1, moveDirectionsPerRing);
	const float angleStep = 2.f * PI / static_cast<float>(numDirs);

	for (float ratio : radiusRatios)
	{
		const float ringR = casterBudgetCm * ratio;
		if (ringR < 1.f) continue;

		for (int32 i = 0; i < numDirs; ++i)
		{
			const float theta = i * angleStep;
			const FVector sample(
				casterLoc.X + ringR * FMath::Cos(theta),
				casterLoc.Y + ringR * FMath::Sin(theta),
				casterLoc.Z);

			float pathLenCm = 0.f;
			if (!UAINavigationHelper::CanReach(ownerCharacter, sample, pathLenCm)) continue;

			FAIAction action;
			action.CastFrom = sample;
			action.PathLengthCm = pathLenCm;
			action.IncomingDangerExpected = ComputeIncomingDangerAt(sample);
			action.Type = EAIActionType::Move;
			OutCandidates.Add(action);
		}
	}
}

void UUtilityAIComponent::EnumerateActions(bool bExcludeMove, TArray<FAIAction>& OutCandidates) const
{
	OutCandidates.Reset();
	if (!ownerCharacter) return;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) { EnumerateWaitAction(OutCandidates); return; }

	//공격 대상은 살아있는 아군
	TArray<ACharacterBase*> targets;
	for (AAllyCharacterBase* a : gm->GetAllies())
	{
		if (IsValid(a) && !a->IsDead()) targets.Add(a);
	}

	USkillComponent* sc = ownerCharacter->GetSkillComponent();
	TArray<UActiveSkillBase*> skills = sc ? sc->GetActiveSkills() : TArray<UActiveSkillBase*>();

	//제자리 공격 후보
	for (UActiveSkillBase* skill : skills)
	{
		if (!skill || !skill->CanExecute()) continue;
		for (ACharacterBase* t : targets)
		{
			EnumerateAttackActionsForTarget(skill, t, false, OutCandidates);
		}
	}

	//이동 후 공격과 순수 이동 후보
	const bool bCanMoveMore = !bExcludeMove && (ownerCharacter->GetCurrentMovingPoint() > 0.f);
	if (bCanMoveMore)
	{
		//이동 후 스탯으로 후보 평가
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
				EnumerateAttackActionsForTarget(skill, t, true, OutCandidates);
			}
		}

		EnumerateMoveActions(OutCandidates);

		if (!prevIsMoved)
		{
			ownerCharacter->OnMoveStateChanged(false);   //복구
		}
	}

	//대기 후보 추가
	EnumerateWaitAction(OutCandidates);
}

const FAIAction* UUtilityAIComponent::ScoreAndPickBest(const TArray<FAIAction>& Candidates, TArray<float>* OutScores) const
{
	if (Candidates.Num() == 0) return nullptr;
	if (OutScores) OutScores->SetNumUninitialized(Candidates.Num());

	const FAIAction* bestPtr = nullptr;
	float bestScore = -TNumericLimits<float>::Max();

	for (int32 i = 0; i < Candidates.Num(); ++i)
	{
		const float s = ScoreAction(Candidates[i]);
		if (OutScores) (*OutScores)[i] = s;
		if (s > bestScore)
		{
			bestScore = s;
			bestPtr = &Candidates[i];
		}
	}
	return bestPtr;
}

float UUtilityAIComponent::ScoreAction(const FAIAction& Action) const
{
	//성향 미지정 시 기본값 사용
	const UAIPersonalityData* p = personalityData
		? personalityData.Get()
		: GetDefault<UAIPersonalityData>();

	float score = 0.f;

	if (Action.IsWait())
	{
		//대기는 고정 점수 사용
		score = p->weightWait;
	}
	else if (Action.IsMove())
	{
		//순수 이동 점수
		score += Action.PathLengthCm * p->weightMoveAdvance;
		score += Action.PathLengthCm * p->weightDistancePenalty;
	}
	else
	{
		//공격과 지원 점수
		const float hitP  = Action.Preview.HitChance / 100.f;
		const float critP = Action.Preview.CritChance / 100.f;

		//기대 피해 계산
		const float expected = hitP * ((1.f - critP) * Action.Preview.NormalDamage
		                              + critP * Action.Preview.CritDamage);
		score += expected * p->weightExpectedDamage;

		//명중률 보너스
		score += hitP * p->weightHitChance;

		//처치 보너스
		if (Action.Preview.bCanKill)
		{
			score += p->weightCanKillBonus * hitP;
		}
		if (Action.Preview.bCanCritKill)
		{
			score += p->weightCanCritKillBonus * hitP * critP;
		}

		//이동 거리 페널티
		score += Action.PathLengthCm * p->weightDistancePenalty;

		//자원 비용 페널티
		if (Action.Skill)
		{
			const int32 resCost = Action.Skill->actionPointCost + Action.Skill->battleResourceCost;
			score += resCost * p->weightResourceCost;
		}

		//대상 기반 점수
		if (Action.Type == EAIActionType::Attack && Action.Target)
		{
			//오버킬 페널티
			const int32 targetHp = Action.Target->GetHp();
			const float overkill = FMath::Max(0.f, static_cast<float>(Action.Preview.NormalDamage - targetHp));
			score += overkill * p->weightOverkillPenalty;

			//체력 낮은 대상 보너스
			const int32 targetMaxHp = Action.Target->GetMaxHp();
			if (targetMaxHp > 0)
			{
				const float missingRatio = 1.f - static_cast<float>(targetHp) / static_cast<float>(targetMaxHp);
				score += missingRatio * p->weightTargetLowHp;
			}

			//대상 위협도 보너스
			if (UWorld* w = GetWorld())
			{
				if (ABattleGameMode* gm = w->GetAuthGameMode<ABattleGameMode>())
				{
					if (AAllyCharacterBase* allyTarget = Cast<AAllyCharacterBase>(Action.Target))
					{
						const FPlayerThreatProfile& tp = gm->GetPlayerThreatProfile(allyTarget);
						const float tHitP  = tp.Accuracy / 100.f;
						const float tCritP = tp.CritChance / 100.f;
						const float targetThreat = tHitP * ((1.f - tCritP) * tp.NormalDamage + tCritP * tp.CritDamage);
						score += targetThreat * p->weightTargetThreat;
					}
				}
			}
		}
	}

	//예상 피격 피해 반영
	score += Action.IncomingDangerExpected * p->weightIncomingDanger;

	//행동 후 잔여 자원 반영
	if (ownerCharacter)
	{
		int32 postAp  = ownerCharacter->GetCurrentActionPoint();
		int32 postRes = ownerCharacter->GetBattleResource();
		if (!Action.IsMove() && !Action.IsWait() && Action.Skill)
		{
			postAp  -= Action.Skill->actionPointCost;
			postRes -= Action.Skill->battleResourceCost;
		}
		score += (postAp + postRes) * p->weightRemainingResource;
	}

	//성향 노이즈 반영
	if (p->noiseTemperature > 0.f)
	{
		score += FMath::FRandRange(-p->noiseTemperature, p->noiseTemperature);
	}

	return score;
}

void UUtilityAIComponent::ExecuteTurn()
{
	if (!ownerCharacter) { FinishTurn(); return; }

	//턴 시작 상태 초기화
	currentIter = 0;
	lastWasMove = false;
	bHasPendingAfterMove = false;

	StepNext();
}

void UUtilityAIComponent::StepNext()
{
	if (!ownerCharacter) { FinishTurn(); return; }

	//행동 수 상한 검사
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
	EnumerateActions(lastWasMove, candidates);
	if (candidates.Num() == 0) { FinishTurn(); return; }

	TArray<float> scores;
	const FAIAction* best = ScoreAndPickBest(candidates, &scores);
	DrawDebugCandidates(candidates, scores, best);
	if (!best || best->IsWait()) { FinishTurn(); return; }

	++currentIter;

	//행동 종류별 실행
	if (best->IsMove())
	{
		//순수 이동 후 이동 후보 제외
		lastWasMove = true;
		bHasPendingAfterMove = false;
		StartMoveTo(best->CastFrom, best->PathLengthCm);
		return;
	}

	if (best->PathLengthCm > 0.f)
	{
		//이동 후 공격 예약
		lastWasMove = false;
		pendingActionAfterMove = *best;
		bHasPendingAfterMove = true;
		StartMoveTo(best->CastFrom, best->PathLengthCm);
		return;
	}

	//제자리 공격 후 다음 행동 예약
	lastWasMove = false;
	ExecuteAttackImmediate(*best);
	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	GetWorld()->GetTimerManager().SetTimer(stepTimerHandle, this, &UUtilityAIComponent::StepNext, actionDelay, false);
}

void UUtilityAIComponent::ExecuteAttackImmediate(const FAIAction& Action)
{
	if (!Action.Skill || !Action.Target || !ownerCharacter) return;

	//대상 방향으로 회전
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

	//중복 콜백 방지
	aic->ReceiveMoveCompleted.RemoveDynamic(this, &UUtilityAIComponent::OnAIMoveCompleted);
	aic->ReceiveMoveCompleted.AddUniqueDynamic(this, &UUtilityAIComponent::OnAIMoveCompleted);

	FAIMoveRequest req(Dest);
	req.SetUsePathfinding(true);
	req.SetAcceptanceRadius(10.f);

	const EPathFollowingRequestResult::Type r = aic->MoveTo(req);

	if (r == EPathFollowingRequestResult::Failed)
	{
		//경로를 찾지 못함, 이동/자원 소모 없이 턴을 안전하게 종료
		bHasPendingAfterMove = false;
		UE_LOG(LogTemp, Warning, TEXT("[UtilityAI] %s MoveTo 실패 — 턴 종료"), *GetNameSafe(ownerCharacter));
		FinishTurn();
		return;
	}

	if (r == EPathFollowingRequestResult::AlreadyAtGoal)
	{
		//이미 목표 지점, 실제 이동이 없었으므로 이동력/이동상태 변화 없이 완료 처리
		OnAIMoveCompleted(FAIRequestID::InvalidRequest, EPathFollowingResult::Success);
		return;
	}

	//실제 이동 요청이 접수된 경우에만 이동 상태 반영 및 이동력 차감
	ownerCharacter->OnMoveStateChanged(true);
	ownerCharacter->ConsumeMovingPoint(PathLengthCm / 100.f);
}

void UUtilityAIComponent::OnAIMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	//이동이 중단/실패한 경우 예약된 공격을 취소하고 턴을 안전하게 종료
	if (Result != EPathFollowingResult::Success)
	{
		bHasPendingAfterMove = false;
		UE_LOG(LogTemp, Warning, TEXT("[UtilityAI] %s 이동 실패(Result=%d) — 턴 종료"),
			*GetNameSafe(ownerCharacter), static_cast<int32>(Result));
		FinishTurn();
		return;
	}

	//이동 완료 패시브 발동
	if (UWorld* world = GetWorld())
	{
		if (ABattleGameMode* gm = world->GetAuthGameMode<ABattleGameMode>())
		{
			gm->BroadcastMoveComplete();
		}
	}

	//예약된 이동 후 공격 처리
	if (bHasPendingAfterMove)
	{
		bHasPendingAfterMove = false;
		ExecuteAttackImmediate(pendingActionAfterMove);
	}

	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	GetWorld()->GetTimerManager().SetTimer(stepTimerHandle, this, &UUtilityAIComponent::StepNext, actionDelay, false);
}

bool UUtilityAIComponent::ShouldStopAfterStep() const
{
	if (!ownerCharacter) return true;
	const int32 ap = ownerCharacter->GetCurrentActionPoint();
	const float mp = ownerCharacter->GetCurrentMovingPoint();
	//자원 변화 없으면 종료
	if (ap == apBefore && FMath::IsNearlyEqual(mp, moveBefore)) return true;
	//자원 소진 시 종료
	if (ap <= 0 && mp <= 0.f) return true;
	return false;
}

void UUtilityAIComponent::FinishTurn()
{
	//대기 타이머 정리
	if (UWorld* world = GetWorld())
	{
		world->GetTimerManager().ClearTimer(stepTimerHandle);
	}

	//이동 완료 콜백 해제
	if (ownerCharacter)
	{
		if (AAIController* aic = Cast<AAIController>(ownerCharacter->GetController()))
		{
			aic->ReceiveMoveCompleted.RemoveDynamic(this, &UUtilityAIComponent::OnAIMoveCompleted);
		}
	}
	OnTurnComplete.Broadcast();
}

//지정 위치의 예상 피격 피해 계산
float UUtilityAIComponent::ComputeIncomingDangerAt(const FVector& AtLocation) const
{
	if (!ownerCharacter) return 0.f;

	//위험도 미사용 성향은 생략
	const UAIPersonalityData* p = personalityData
		? personalityData.Get()
		: GetDefault<UAIPersonalityData>();
	if (FMath::IsNearlyZero(p->weightIncomingDanger)) return 0.f;

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return 0.f;

	const FVector eye(0.f, 0.f, 80.f);   //AI 시야 높이
	float total = 0.f;

	for (AAllyCharacterBase* threat : gm->GetAllies())
	{
		if (!IsValid(threat) || threat->IsDead()) continue;

		const FPlayerThreatProfile& prof = gm->GetPlayerThreatProfile(threat);
		const FVector threatLoc = threat->GetActorLocation();

		//사거리 밖 위협 제외
		if (prof.RangeCm > 0.f && FVector::Dist(threatLoc, AtLocation) > prof.RangeCm) continue;

		//시야선 없는 위협 제외
		FCollisionQueryParams params(SCENE_QUERY_STAT(AI_IncomingDangerLOS), false, threat);
		FHitResult hit;
		const bool bBlocked = world->LineTraceSingleByChannel(
			hit, threatLoc + eye, AtLocation + eye, ECC_Visibility, params);
		if (bBlocked && hit.GetActor() != ownerCharacter) continue;   //시야 차단

		//기대 피해 합산
		const float hitP  = prof.Accuracy / 100.f;
		const float critP = prof.CritChance / 100.f;
		total += hitP * ((1.f - critP) * prof.NormalDamage + critP * prof.CritDamage);
	}
	return total;
}

void UUtilityAIComponent::DrawDebugCandidates(const TArray<FAIAction>& Candidates,
                                              const TArray<float>& Scores,
                                              const FAIAction* Best) const
{
	if (CVarUtilityAIDebug.GetValueOnGameThread() == 0) return;
	UWorld* world = GetWorld();
	if (!world || Candidates.Num() != Scores.Num()) return;

	const float duration = 6.f;
	const FVector textOffset(0.f, 0.f, 70.f);

	for (int32 i = 0; i < Candidates.Num(); ++i)
	{
		const FAIAction& c = Candidates[i];
		const float score = Scores[i];

		//행동 종류별 색상
		FColor color;
		switch (c.Type)
		{
			case EAIActionType::Attack:  color = FColor::Red;    break;
			case EAIActionType::Support: color = FColor::Green;  break;
			case EAIActionType::Move:    color = FColor::Blue;   break;
			case EAIActionType::Wait:    color = FColor::Yellow; break;
			default:                     color = FColor::White;  break;
		}

		const bool bIsBest = (Best && &c == Best);
		const float radius = bIsBest ? 55.f : 25.f;

		DrawDebugSphere(world, c.CastFrom + FVector(0.f, 0.f, 5.f),
			radius, 12, color, false, duration, 0, bIsBest ? 4.f : 1.5f);

		//선택 후보 점수 강조
		const FString label = bIsBest
			? FString::Printf(TEXT(">> %.1f  (D=%.1f Path=%.0f)"),
				score, c.IncomingDangerExpected, c.PathLengthCm)
			: FString::Printf(TEXT("%.1f"), score);
		DrawDebugString(world, c.CastFrom + textOffset, label,
			nullptr, color, duration, false, bIsBest ? 2.2f : 1.4f);

		//공격과 지원 의도선 표시
		if ((c.Type == EAIActionType::Attack || c.Type == EAIActionType::Support) && c.Target)
		{
			DrawDebugLine(world, c.CastFrom + FVector(0.f, 0.f, 80.f),
				c.Target->GetActorLocation() + FVector(0.f, 0.f, 80.f),
				color, false, duration, 0, bIsBest ? 2.f : 0.5f);
		}
	}
}

void UUtilityAIComponent::EnumerateWaitAction(TArray<FAIAction>& OutCandidates) const
{
	if (!ownerCharacter) return;

	FAIAction action;
	action.Type = EAIActionType::Wait;
	action.CastFrom = ownerCharacter->GetActorLocation();
	action.IncomingDangerExpected = ComputeIncomingDangerAt(action.CastFrom);
	OutCandidates.Add(action);
}
