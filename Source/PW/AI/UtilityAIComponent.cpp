//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UtilityAIComponent.h"
#include "AI/AINavigationHelper.h"
#include "AI/AIPersonalityData.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
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

void UUtilityAIComponent::ResolvePickTargets(EPickTeam Team, TArray<ACharacterBase*>& Out) const
{
	Out.Reset();
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm || !ownerCharacter) return;

	//시전자의 적 = 플레이어 아군
	if (Team == EPickTeam::EnemyOnly || Team == EPickTeam::Any)
	{
		for (AAllyCharacterBase* a : gm->GetAllies())
		{
			if (IsValid(a) && !a->IsDead()) Out.Add(a);
		}
	}
	//시전자의 아군 = AI 진영, 자기 자신은 제외(범위 규칙과 일치)
	if (Team == EPickTeam::AllyOnly || Team == EPickTeam::Any)
	{
		for (AEnemyBase* e : gm->GetEnemies())
		{
			if (IsValid(e) && !e->IsDead() && e != ownerCharacter) Out.Add(e);
		}
	}
}

void UUtilityAIComponent::CollectAllCharacters(TArray<ACharacterBase*>& Out) const
{
	Out.Reset();
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return;

	for (AAllyCharacterBase* a : gm->GetAllies())
	{
		if (IsValid(a) && !a->IsDead()) Out.Add(a);
	}
	for (AEnemyBase* e : gm->GetEnemies())
	{
		if (IsValid(e) && !e->IsDead()) Out.Add(e);
	}
}

float UUtilityAIComponent::AreaOverlapRadius(const UActiveSkillBase* Skill)
{
	if (!Skill) return 0.f;
	//인디케이터의 overlapSphere 반경 규칙을 그대로 미러링
	switch (Skill->areaForm)
	{
	case EAreaForm::Circle: return Skill->areaParameter1;
	case EAreaForm::Cone:   return Skill->areaParameter1;
	case EAreaForm::Ray:    return FMath::Max(Skill->areaParameter1, Skill->areaParameter2) * 0.5f;
	}
	return 0.f;
}

bool UUtilityAIComponent::PassesAreaTargetFilter(const UActiveSkillBase* Skill, const ACharacterBase* Character) const
{
	if (!Skill || !Character || !ownerCharacter) return false;

	//시전자 기준 같은 진영 여부
	const bool bSameTeam = (Character->IsAlly() == ownerCharacter->IsAlly());
	switch (Skill->areaTarget)
	{
	case EAreaTarget::EnemyOnly: return !bSameTeam;
	case EAreaTarget::AllyOnly:  return bSameTeam;
	case EAreaTarget::All:       return true;
	case EAreaTarget::None:      return true;
	}
	return false;
}

void UUtilityAIComponent::CollectAreaAffected(const UActiveSkillBase* Skill, const FVector& AimLoc,
                                              TArray<ACharacterBase*>& Out) const
{
	Out.Reset();
	if (!Skill || !ownerCharacter) return;

	const float radiusCm = AreaOverlapRadius(Skill);
	if (radiusCm <= 0.f) return;

	TArray<ACharacterBase*> all;
	CollectAllCharacters(all);
	for (ACharacterBase* c : all)
	{
		if (c == ownerCharacter) continue;   //시전자 제외
		if (FVector::Dist(c->GetActorLocation(), AimLoc) > radiusCm) continue;
		if (!PassesAreaTargetFilter(Skill, c)) continue;
		Out.Add(c);
	}
}

bool UUtilityAIComponent::HasLOSToLocation(const FVector& FromLoc, const FVector& AimLoc) const
{
	UWorld* world = GetWorld();
	if (!world || !ownerCharacter) return false;

	const FVector eye(0.f, 0.f, 80.f);
	FCollisionQueryParams params(SCENE_QUERY_STAT(AI_GroundLOS), false, ownerCharacter);
	FHitResult hit;
	const bool bBlocked = world->LineTraceSingleByChannel(
		hit, FromLoc + eye, AimLoc + eye, ECC_Visibility, params);
	//지면 지점은 캐릭터가 아니므로 무엇이든 막히면 시야 차단으로 간주
	return !bBlocked;
}

void UUtilityAIComponent::GatherCastPositions(const FVector& AimLoc, float PickRangeCm, bool bAssumeMoved,
                                              TArray<TPair<FVector, float>>& Out, bool bGateAimRange) const
{
	Out.Reset();
	if (!ownerCharacter) return;

	const FVector casterLoc = ownerCharacter->GetActorLocation();
	const float budgetCm = ownerCharacter->GetCurrentMovingPoint() * 100.f;

	TArray<FVector> samples;
	if (bAssumeMoved)
	{
		//이동 후 후보는 aim 주변 샘플, 사거리 무제한이면 제자리만
		if (PickRangeCm > 0.f)
		{
			FibonacciDiskSample(AimLoc, PickRangeCm, attackPositionSamples, samples);
		}
		else
		{
			samples.Add(casterLoc);
		}
	}
	else
	{
		//제자리 후보는 현재 위치만
		samples.Add(casterLoc);
	}

	for (const FVector& s : samples)
	{
		//사거리 밖 후보 제외, 멀티픽은 픽별 검사에 위임하므로 게이트 생략
		if (bGateAimRange && PickRangeCm > 0.f && FVector::Dist(s, AimLoc) > PickRangeCm) continue;

		float pathLenCm = 0.f;
		if (bAssumeMoved)
		{
			//잔여 이동력 내 도달 가능 여부
			if (FVector::Dist(casterLoc, s) > budgetCm) continue;
			if (!UAINavigationHelper::CanReach(ownerCharacter, s, pathLenCm)) continue;
		}
		Out.Emplace(s, pathLenCm);
	}
}

void UUtilityAIComponent::BuildActionFromCast(UActiveSkillBase* Skill, const FVector& CastFrom, float PathLenCm,
                                              const TArray<ACharacterBase*>& Affected, bool bHeal,
                                              TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !ownerCharacter || Affected.Num() == 0) return;

	const bool casterIsAlly = ownerCharacter->IsAlly();

	//데미지 집계
	float enemyExpected = 0.f;
	float allyExpected = 0.f;
	int32 enemyCount = 0;
	float hitChanceSum = 0.f;
	float expectedKills = 0.f;
	float expectedCritKills = 0.f;
	float overkillTotal = 0.f;

	//회복 집계
	float healTotal = 0.f;
	float healLowHpCount = 0.f;
	float healExtremeLowHpCount = 0.f;

	//대표 선정용, 데미지=최대 피해 적군 / 회복=가장 많이 부상한 아군
	ACharacterBase* primary = nullptr;
	float bestKey = -1.f;
	FDamageResult primaryPreview;

	//나머지 영향 대상 보관, 대표를 [0]에 두기 위해 분리 수집
	TArray<ACharacterBase*> others;

	for (ACharacterBase* c : Affected)
	{
		if (!IsValid(c)) continue;

		const bool bSameTeam = (c->IsAlly() == casterIsAlly);
		const FDamageResult pr = c->PreviewDamage(Skill, ownerCharacter, CastFrom);
		const float hitP = pr.HitChance / 100.f;

		if (bHeal)
		{
			//회복은 같은 진영 부상 대상만 의미
			if (!bSameTeam) continue;
			const int32 maxHp = c->GetMaxHp();
			const int32 missing = maxHp - c->GetHp();
			if (missing <= 0 || maxHp <= 0) continue;   //풀피는 오버힐이라 제외

			//회복은 음수 데미지, 부호 반전해 회복량으로 사용
			const float healAmount = -static_cast<float>(pr.NormalDamage);
			if (healAmount <= 0.f) continue;

			//오버힐 제외한 유효 회복량 집계
			healTotal += hitP * FMath::Min(healAmount, static_cast<float>(missing));

			//체력 비율 구간별 가중, 배타적 구간(20% 미만은 extreme만, 20~50%는 low만)
			const float ratio = static_cast<float>(c->GetHp()) / static_cast<float>(maxHp);
			if (ratio < 0.2f)      healExtremeLowHpCount += 1.f;
			else if (ratio < 0.5f) healLowHpCount += 1.f;

			if (static_cast<float>(missing) > bestKey)
			{
				if (primary) others.Add(primary);
				primary = c;
				bestKey = static_cast<float>(missing);
				primaryPreview = pr;
			}
			else
			{
				others.Add(c);
			}
		}
		else
		{
			const float critP = pr.CritChance / 100.f;
			float expected = hitP * ((1.f - critP) * pr.NormalDamage + critP * pr.CritDamage);
			if (expected < 0.f) expected = 0.f;

			if (bSameTeam)
			{
				//아군 오사 피해 별도 집계
				allyExpected += expected;
				others.Add(c);
			}
			else
			{
				enemyExpected += expected;

				//적군 대상별 명중·처치·오버킬 집계
				++enemyCount;
				hitChanceSum += pr.HitChance;
				if (pr.bCanKill)     expectedKills += hitP;
				if (pr.bCanCritKill) expectedCritKills += hitP * critP;
				overkillTotal += FMath::Max(0.f, static_cast<float>(pr.NormalDamage - c->GetHp()));

				if (expected > bestKey)
				{
					if (primary) others.Add(primary);
					primary = c;
					bestKey = expected;
					primaryPreview = pr;
				}
				else
				{
					others.Add(c);
				}
			}
		}
	}

	//유효 대상이 없으면 후보 미생성 (데미지=적군 없음, 회복=부상 아군 없음)
	if (!primary) return;

	FAIAction action;
	action.Skill = Skill;
	action.CastFrom = CastFrom;
	action.PathLengthCm = PathLenCm;
	action.Preview = primaryPreview;
	action.IncomingDangerExpected = ComputeIncomingDangerAt(CastFrom);

	if (bHeal)
	{
		action.Type = EAIActionType::Support;
		action.HealExpectedTotal = healTotal;
		action.HealLowHpCount = healLowHpCount;
		action.HealExtremeLowHpCount = healExtremeLowHpCount;
	}
	else
	{
		action.Type = EAIActionType::Attack;
		action.EnemyExpectedDamage = enemyExpected;
		action.AllyExpectedDamage = allyExpected;
		action.AvgEnemyHitChance = (enemyCount > 0) ? hitChanceSum / enemyCount : 0.f;
		action.ExpectedKills = expectedKills;
		action.ExpectedCritKills = expectedCritKills;
		action.OverkillTotal = overkillTotal;
	}

	//대표를 맨 앞에, 이어서 나머지 영향 대상
	action.Targets.Add(primary);
	for (ACharacterBase* c : others)
	{
		action.Targets.Add(c);
	}

	OutCandidates.Add(action);
}

void UUtilityAIComponent::EnumerateSingleTarget(UActiveSkillBase* Skill, ACharacterBase* Target,
                                                bool bAssumeMoved, bool bHeal, TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !Target || !ownerCharacter || Target->IsDead()) return;

	const FVector aimLoc = Target->GetActorLocation();

	TArray<TPair<FVector, float>> positions;
	GatherCastPositions(aimLoc, Skill->pickRange, bAssumeMoved, positions);

	for (const TPair<FVector, float>& pos : positions)
	{
		//대표 대상에 대한 시야선 필요
		if (!UAINavigationHelper::HasLineOfSightFrom(ownerCharacter, pos.Key, Target)) continue;

		TArray<ACharacterBase*> affected;
		if (Skill->areaTarget != EAreaTarget::None)
		{
			//범위 스킬은 대상 주변을 오버랩, 대표 대상 포함 보장
			CollectAreaAffected(Skill, aimLoc, affected);
			affected.AddUnique(Target);
		}
		else
		{
			affected.Add(Target);
		}

		BuildActionFromCast(Skill, pos.Key, pos.Value, affected, bHeal, OutCandidates);
	}
}

void UUtilityAIComponent::EnumerateMultiPick(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& PickTargets,
                                             bool bAssumeMoved, bool bHeal, TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !ownerCharacter || PickTargets.Num() == 0) return;

	//대상 중심(centroid)을 aim으로 시전 위치 샘플
	FVector centroid = FVector::ZeroVector;
	for (ACharacterBase* t : PickTargets)
	{
		centroid += t->GetActorLocation();
	}
	centroid /= static_cast<float>(PickTargets.Num());

	//centroid는 위치 샘플 중심일 뿐이라 aim-사거리 게이트는 끄고, 픽별 사거리로 유효성 판정
	TArray<TPair<FVector, float>> positions;
	GatherCastPositions(centroid, Skill->pickRange, bAssumeMoved, positions, false);

	for (const TPair<FVector, float>& pos : positions)
	{
		//시전 위치에서 사거리·시야선 확보된 픽 후보를 효용(회복량 또는 피해)으로 정렬
		TArray<TPair<ACharacterBase*, float>> cands;
		for (ACharacterBase* t : PickTargets)
		{
			if (!IsValid(t) || t->IsDead()) continue;
			if (Skill->pickRange > 0.f && FVector::Dist(pos.Key, t->GetActorLocation()) > Skill->pickRange) continue;
			if (!UAINavigationHelper::HasLineOfSightFrom(ownerCharacter, pos.Key, t)) continue;

			const FDamageResult pr = t->PreviewDamage(Skill, ownerCharacter, pos.Key);
			const float hitP  = pr.HitChance / 100.f;
			float value;
			if (bHeal)
			{
				//회복 효용은 오버힐 제외한 유효 회복량, 풀피는 후보 제외
				const int32 missing = t->GetMaxHp() - t->GetHp();
				if (missing <= 0) continue;
				const float healAmount = -static_cast<float>(pr.NormalDamage);
				value = hitP * FMath::Min(healAmount, static_cast<float>(missing));
			}
			else
			{
				const float critP = pr.CritChance / 100.f;
				value = hitP * ((1.f - critP) * pr.NormalDamage + critP * pr.CritDamage);
			}
			cands.Emplace(t, value);
		}
		if (cands.Num() == 0) continue;

		cands.Sort([](const TPair<ACharacterBase*, float>& A, const TPair<ACharacterBase*, float>& B)
		{
			return A.Value > B.Value;
		});

		//상위 pickCount개 선택, 범위 스킬이면 각 픽의 오버랩까지 합류
		const int32 takeN = FMath::Min(Skill->pickCount, cands.Num());
		TArray<ACharacterBase*> affected;
		for (int32 i = 0; i < takeN; ++i)
		{
			affected.AddUnique(cands[i].Key);
			if (Skill->areaTarget != EAreaTarget::None)
			{
				TArray<ACharacterBase*> areaHit;
				CollectAreaAffected(Skill, cands[i].Key->GetActorLocation(), areaHit);
				for (ACharacterBase* h : areaHit)
				{
					affected.AddUnique(h);
				}
			}
		}

		BuildActionFromCast(Skill, pos.Key, pos.Value, affected, bHeal, OutCandidates);
	}
}

void UUtilityAIComponent::EnumerateGroundPoint(UActiveSkillBase* Skill, bool bAssumeMoved,
                                               bool bHeal, TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !ownerCharacter) return;
	//지면 대상 스킬은 범위가 없으면 맞출 대상이 없음
	if (Skill->areaTarget == EAreaTarget::None) return;

	//EAreaTarget에 부합하는 캐릭터를 seed로, 위치와 쌍 중점을 지면점 후보로
	TArray<ACharacterBase*> all;
	CollectAllCharacters(all);
	TArray<ACharacterBase*> eligible;
	for (ACharacterBase* c : all)
	{
		if (c == ownerCharacter) continue;
		if (PassesAreaTargetFilter(Skill, c)) eligible.Add(c);
	}
	if (eligible.Num() == 0) return;

	TArray<FVector> aimPoints;
	for (ACharacterBase* c : eligible)
	{
		aimPoints.Add(c->GetActorLocation());
	}
	//대상 쌍 중점, 두 대상을 한 번에 덮는 지점 탐색
	for (int32 i = 0; i < eligible.Num(); ++i)
	{
		for (int32 j = i + 1; j < eligible.Num(); ++j)
		{
			aimPoints.Add((eligible[i]->GetActorLocation() + eligible[j]->GetActorLocation()) * 0.5f);
		}
	}

	for (const FVector& aim : aimPoints)
	{
		TArray<TPair<FVector, float>> positions;
		GatherCastPositions(aim, Skill->pickRange, bAssumeMoved, positions);

		for (const TPair<FVector, float>& pos : positions)
		{
			if (!HasLOSToLocation(pos.Key, aim)) continue;

			TArray<ACharacterBase*> affected;
			CollectAreaAffected(Skill, aim, affected);
			BuildActionFromCast(Skill, pos.Key, pos.Value, affected, bHeal, OutCandidates);
		}
	}
}

void UUtilityAIComponent::EnumerateSkillActions(UActiveSkillBase* Skill, bool bAssumeMoved,
                                                TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !ownerCharacter) return;

	const bool bHeal = (Skill->skillType == ESkillType::Heal);

	//회복 외 지원 스킬(Buff/Ailment)은 전용 점수 축이 없어 이번 단계 미열거
	//TODO: 버프/상태이상 점수 축 구현 시 열거 활성화
	if (!bHeal && (Skill->skillType == ESkillType::Buff || Skill->skillType == ESkillType::Ailment))
	{
		return;
	}

	//Self 대상: 자기 회복만 처리, 그 외 Self 지원 스킬은 보류
	if (Skill->selectMode == ESelectMode::Self)
	{
		if (!bHeal) return;
		//자기 회복은 제자리 후보로 충분, 이동 패스에서는 생략
		if (bAssumeMoved) return;
		TArray<ACharacterBase*> selfOnly;
		selfOnly.Add(ownerCharacter);
		BuildActionFromCast(Skill, ownerCharacter->GetActorLocation(), 0.f, selfOnly, true, OutCandidates);
		return;
	}

	if (Skill->selectMode == ESelectMode::GroundPoint)
	{
		EnumerateGroundPoint(Skill, bAssumeMoved, bHeal, OutCandidates);
		return;
	}

	//SinglePick, pickTeam으로 대상 집합 결정
	TArray<ACharacterBase*> pickTargets;
	ResolvePickTargets(Skill->pickTeam, pickTargets);
	if (pickTargets.Num() == 0) return;

	if (Skill->pickCount > 1)
	{
		EnumerateMultiPick(Skill, pickTargets, bAssumeMoved, bHeal, OutCandidates);
	}
	else
	{
		for (ACharacterBase* t : pickTargets)
		{
			EnumerateSingleTarget(Skill, t, bAssumeMoved, bHeal, OutCandidates);
		}
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

	USkillComponent* sc = ownerCharacter->GetSkillComponent();
	TArray<UActiveSkillBase*> skills = sc ? sc->GetActiveSkills() : TArray<UActiveSkillBase*>();

	//제자리 스킬 후보, 대상 집합은 각 스킬의 pickTeam/selectMode/EAreaTarget으로 결정
	for (UActiveSkillBase* skill : skills)
	{
		if (!skill || !skill->CanExecute()) continue;
		EnumerateSkillActions(skill, false, OutCandidates);
	}

	//이동 후 스킬과 순수 이동 후보
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
			EnumerateSkillActions(skill, true, OutCandidates);
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
		//공격과 지원 점수, 대상별 집계값 사용
		ACharacterBase* primary = Action.GetPrimaryTarget();

		//적군 누적 기대 피해 보너스, 아군 오사 기대 피해 페널티
		score += Action.EnemyExpectedDamage * p->weightExpectedDamage;
		score += Action.AllyExpectedDamage * p->weightAllyDamagePenalty;

		//명중률 보너스, 적군 대상 평균 명중률
		score += (Action.AvgEnemyHitChance / 100.f) * p->weightHitChance;

		//처치 보너스, 대상별 기대 처치 수만큼 가중
		score += p->weightCanKillBonus * Action.ExpectedKills;
		score += p->weightCanCritKillBonus * Action.ExpectedCritKills;

		//오버킬 페널티, 대상별 오버킬 합산
		score += Action.OverkillTotal * p->weightOverkillPenalty;

		//회복 점수, 데미지 후보는 회복 집계가 0이라 영향 없음
		score += Action.HealExpectedTotal * p->weightExpectedHeal;
		score += Action.HealLowHpCount * p->weightHealTargetLowHp;
		score += Action.HealExtremeLowHpCount * p->weightHealTargetExtremeLowHp;

		//이동 거리 페널티
		score += Action.PathLengthCm * p->weightDistancePenalty;

		//자원 비용 페널티
		if (Action.Skill)
		{
			const int32 resCost = Action.Skill->actionPointCost + Action.Skill->battleResourceCost;
			score += resCost * p->weightResourceCost;
		}

		//대표 대상 기반 점수
		if (Action.Type == EAIActionType::Attack && primary)
		{
			//체력 낮은 대상 보너스
			const int32 targetHp = primary->GetHp();
			const int32 targetMaxHp = primary->GetMaxHp();
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
					if (AAllyCharacterBase* allyTarget = Cast<AAllyCharacterBase>(primary))
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
	ACharacterBase* primary = Action.GetPrimaryTarget();
	if (!Action.Skill || !primary || !ownerCharacter) return;

	//대표 대상 방향으로 회전
	if (Action.Skill->selectMode != ESelectMode::Self)
	{
		FVector dir = primary->GetActorLocation() - ownerCharacter->GetActorLocation();
		dir.Z = 0.f;
		if (!dir.IsNearlyZero())
		{
			ownerCharacter->SetActorRotation(dir.Rotation());
		}
	}

	if (USkillComponent* sc = ownerCharacter->GetSkillComponent())
	{
		//유효한 영향 대상만 추려 다중 대상 시전
		TArray<ACharacterBase*> targets;
		for (const TObjectPtr<ACharacterBase>& t : Action.Targets)
		{
			if (IsValid(t)) targets.Add(t);
		}

		//회복은 회복량, 그 외는 적군 기대 피해로 로그 표기
		if (Action.Type == EAIActionType::Support)
		{
			UE_LOG(LogTemp, Log, TEXT("[UtilityAI] %s 회복 스킬 사용: %s → 대표 %s, 대상 %d명 (기대 회복량 %.1f)"),
				*GetNameSafe(ownerCharacter), *Action.Skill->skillName.ToString(),
				*GetNameSafe(primary), targets.Num(), Action.HealExpectedTotal);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UtilityAI] %s 스킬 사용: %s → 대표 %s, 대상 %d명 (적군 기대피해 %.1f)"),
				*GetNameSafe(ownerCharacter), *Action.Skill->skillName.ToString(),
				*GetNameSafe(primary), targets.Num(), Action.EnemyExpectedDamage);
		}

		sc->DirectExecute(Action.Skill, targets);
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

		//공격과 지원 의도선 표시, 영향 대상마다 한 줄씩
		if (c.Type == EAIActionType::Attack || c.Type == EAIActionType::Support)
		{
			for (const TObjectPtr<ACharacterBase>& t : c.Targets)
			{
				if (!IsValid(t)) continue;
				DrawDebugLine(world, c.CastFrom + FVector(0.f, 0.f, 80.f),
					t->GetActorLocation() + FVector(0.f, 0.f, 80.f),
					color, false, duration, 0, bIsBest ? 2.f : 0.5f);
			}
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
