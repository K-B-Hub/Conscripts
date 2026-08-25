//Fill out your copyright notice in the Description page of Project Settings.

#include "AI/UtilityAIComponent.h"
#include "AI/AINavigationHelper.h"
#include "Actors/Terrain/TerrainBase.h"
#include "AI/AIPersonalityData.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Object/Buff/BuffBase.h"
#include "Object/Ailment/AilmentBase.h"
#include "ActorComponent/BuffComponent.h"
#include "ActorComponent/AilmentComponent.h"
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
	TEXT("Utility AI 후보 점수 시각화. 0=off, 1=행동 라벨+최고점 상세, 2=전 후보 상세"),
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
                                              TArray<FAIAction>& Out, bool bGateAimRange) const
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

		FAIAction pos;
		pos.CastFrom = s;

		if (bAssumeMoved)
		{
			//잔여 이동력 내 도달 가능 여부, 지형 배율은 CanReach가 반영
			if (FVector::Dist(casterLoc, s) > budgetCm) continue;

			FTerrainPathInfo terrainInfo;
			if (!UAINavigationHelper::CanReach(ownerCharacter, s, pos.PathLengthCm, &terrainInfo)) continue;

			pos.TerrainWeightedCostCm = terrainInfo.WeightedCostCm;
			pos.TerrainValueAtDest = SumTerrainStayValue(terrainInfo.DestTerrains);
			pos.TerrainPassThroughValue = SumTerrainPassValue(terrainInfo.PassedTerrains);
		}
		else
		{
			//제자리 후보는 경로가 없으므로 현재 서 있는 지형만 평가, 이동 후보와 같은 저울에 오르도록
			TArray<TObjectPtr<ATerrainBase>> here;
			UAINavigationHelper::GetTerrainsAt(GetWorld(), s, here);
			pos.TerrainValueAtDest = SumTerrainStayValue(here);
		}

		Out.Add(pos);
	}
}

float UUtilityAIComponent::ScoreTerrain(const FAIAction& Action) const
{
	const UAIPersonalityData* p = personalityData
		? personalityData.Get()
		: GetDefault<UAIPersonalityData>();

	//지형이 기술한 객관적 손익, 기대 HP 환산이라 기대 피해와 같은 저울
	const float rawValue = Action.TerrainValueAtDest + Action.TerrainPassThroughValue;
	if (FMath::IsNearlyZero(rawValue) || !ownerCharacter) return 0.f;

	//잃은 체력 비율만큼 손익을 증폭, 만피 1.0 ~ 빈사 2.0에 수렴
	const int32 maxHp = ownerCharacter->GetMaxHp();
	const float missingRatio = (maxHp > 0)
		? 1.f - static_cast<float>(ownerCharacter->GetHp()) / static_cast<float>(maxHp)
		: 0.f;
	const float survivalScale = 1.f + missingRatio;

	return rawValue * survivalScale * p->weightTerrainValue;
}

float UUtilityAIComponent::SumTerrainStayValue(const TArray<TObjectPtr<ATerrainBase>>& Terrains) const
{
	float total = 0.f;
	for (ATerrainBase* terrain : Terrains)
	{
		if (terrain) total += terrain->aiBaseValue;
	}
	return total;
}

float UUtilityAIComponent::SumTerrainPassValue(const TArray<TObjectPtr<ATerrainBase>>& Terrains) const
{
	float total = 0.f;
	for (ATerrainBase* terrain : Terrains)
	{
		if (terrain) total += terrain->aiPassThroughValue;
	}
	return total;
}

float UUtilityAIComponent::ComputeRiderValue(const UActiveSkillBase* Skill, const ACharacterBase* Target, float HitP,
                                             float& OutBuffValue, float& OutAilmentValue) const
{
	OutBuffValue = 0.f;
	OutAilmentValue = 0.f;
	if (!Skill || !Target || !ownerCharacter) return 0.f;

	const bool bSameTeam = (Target->IsAlly() == ownerCharacter->IsAlly());
	const float teamSign = bSameTeam ? 1.f : -1.f;

	//버프는 받는 대상 입장에서 이로운가를 팀 여부와 따져 - or + 점수
	for (const TSubclassOf<UBuffBase>& buffClass : Skill->buffs)
	{
		const UBuffBase* cdo = buffClass ? buffClass.GetDefaultObject() : nullptr;
		if (!cdo) continue;

		//비중첩 버프가 이미 있고 잔여 2턴 이상이면 리필 불가
		if (!cdo->isStackable)
		{
			bool bBlocked = false;
			if (UBuffComponent* bc = Target->GetBuffComponent())
			{
				for (const TObjectPtr<UBuffBase>& active : bc->GetActiveBuffs())
				{
					if (active && active->GetClass() == buffClass && active->remainingTurn >= 2)
					{
						bBlocked = true;
						break;
					}
				}
			}
			if (bBlocked) continue;
		}

		OutBuffValue += HitP * teamSign * cdo->buffTurn * BuffValueForTarget(cdo, Target);
	}

	//상태이상은 아군 -, 적군 + 점수
	for (const TSubclassOf<UAilmentBase>& ailmentClass : Skill->ailments)
	{
		const UAilmentBase* cdo = ailmentClass ? ailmentClass.GetDefaultObject() : nullptr;
		if (!cdo) continue;

		if (!cdo->isStackable)
		{
			bool bBlocked = false;
			if (UAilmentComponent* ac = Target->GetAilmentComponent())
			{
				for (const TObjectPtr<UAilmentBase>& active : ac->GetActiveAilments())
				{
					if (active && active->GetClass() == ailmentClass && active->remainingTurn >= 2)
					{
						bBlocked = true;
						break;
					}
				}
			}
			if (bBlocked) continue;
		}

		OutAilmentValue += HitP * (-teamSign) * cdo->ailmentTurn * AilmentValueForTarget(cdo, Target);
	}

	return OutBuffValue + OutAilmentValue;
}

void UUtilityAIComponent::BuildActionFromCast(UActiveSkillBase* Skill, const FAIAction& Pos,
                                              const TArray<ACharacterBase*>& Affected, const FVector& AimPoint,
                                              TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !ownerCharacter || Affected.Num() == 0) return;

	const FVector& CastFrom = Pos.CastFrom;
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

	//rider 집계
	float buffTotal = 0.f;
	float ailmentTotal = 0.f;

	//대표 후보, 우선순위 = 최대 피해 적군 > 최대 부상 아군 > 최대 rider 가치 대상
	ACharacterBase* bestEnemy = nullptr;
	float bestEnemyKey = -1.f;
	FDamageResult bestEnemyPreview;
	ACharacterBase* bestHeal = nullptr;
	float bestHealKey = 0.f;
	FDamageResult bestHealPreview;
	ACharacterBase* bestRider = nullptr;
	float bestRiderKey = 0.f;
	FDamageResult bestRiderPreview;

	TArray<ACharacterBase*> validTargets;

	for (ACharacterBase* c : Affected)
	{
		if (!IsValid(c)) continue;
		validTargets.Add(c);

		const bool bSameTeam = (c->IsAlly() == casterIsAlly);
		const FDamageResult pr = c->PreviewDamage(Skill, ownerCharacter, CastFrom);
		const float hitP = pr.HitChance / 100.f;

		//직접 피해/회복, 부호로 구분 (회복 스킬은 음수 데미지)
		if (pr.NormalDamage < 0)
		{
			//같은 진영 부상 대상만 유효 회복
			if (bSameTeam)
			{
				const int32 maxHp = c->GetMaxHp();
				const int32 missing = maxHp - c->GetHp();
				if (missing > 0 && maxHp > 0)
				{
					//오버힐 제외한 유효 회복량 집계
					const float healAmount = -static_cast<float>(pr.NormalDamage);
					healTotal += hitP * FMath::Min(healAmount, static_cast<float>(missing));

					//체력 비율 구간별 가중, 배타적 구간(20% 미만은 extreme만, 20~50%는 low만)
					const float ratio = static_cast<float>(c->GetHp()) / static_cast<float>(maxHp);
					if (ratio < 0.2f)      healExtremeLowHpCount += 1.f;
					else if (ratio < 0.5f) healLowHpCount += 1.f;

					if (static_cast<float>(missing) > bestHealKey)
					{
						bestHeal = c;
						bestHealKey = static_cast<float>(missing);
						bestHealPreview = pr;
					}
				}
			}
		}
		else if (pr.NormalDamage > 0)
		{
			const float critP = pr.CritChance / 100.f;
			float expected = hitP * ((1.f - critP) * pr.NormalDamage + critP * pr.CritDamage);
			if (expected < 0.f) expected = 0.f;

			if (bSameTeam)
			{
				//아군 오사 피해 별도 집계
				allyExpected += expected;
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

				if (expected > bestEnemyKey)
				{
					bestEnemy = c;
					bestEnemyKey = expected;
					bestEnemyPreview = pr;
				}
			}
		}

		//rider 버프/상태이상 가치, 스킬 유형과 무관하게 집계
		float buffV = 0.f;
		float ailV = 0.f;
		const float riderV = ComputeRiderValue(Skill, c, hitP, buffV, ailV);
		buffTotal += buffV;
		ailmentTotal += ailV;
		if (riderV > bestRiderKey)
		{
			bestRider = c;
			bestRiderKey = riderV;
			bestRiderPreview = pr;
		}
	}

	//대표 선정, 유효 가치가 전혀 없으면 후보 미생성
	ACharacterBase* primary = bestEnemy ? bestEnemy : (bestHeal ? bestHeal : bestRider);
	if (!primary) return;
	const FDamageResult& primaryPreview = bestEnemy ? bestEnemyPreview : (bestHeal ? bestHealPreview : bestRiderPreview);

	//위치·경로·지형 값은 GatherCastPositions가 채운 것을 그대로 승계
	FAIAction action = Pos;
	action.Skill = Skill;
	action.AimPoint = AimPoint;
	action.Preview = primaryPreview;
	action.IncomingDangerExpected = ComputeIncomingDangerAt(CastFrom);

	//적 대상 직접 피해가 있으면 공격, 그 외는 지원
	action.Type = bestEnemy ? EAIActionType::Attack : EAIActionType::Support;
	action.EnemyExpectedDamage = enemyExpected;
	action.AllyExpectedDamage = allyExpected;
	action.AvgEnemyHitChance = (enemyCount > 0) ? hitChanceSum / enemyCount : 0.f;
	action.ExpectedKills = expectedKills;
	action.ExpectedCritKills = expectedCritKills;
	action.OverkillTotal = overkillTotal;
	action.HealExpectedTotal = healTotal;
	action.HealLowHpCount = healLowHpCount;
	action.HealExtremeLowHpCount = healExtremeLowHpCount;
	action.BuffValueTotal = buffTotal;
	action.AilmentValueTotal = ailmentTotal;

	//대표를 맨 앞에, 이어서 나머지 영향 대상 (실행 시 DirectExecute 대상 집합)
	action.Targets.Add(primary);
	for (ACharacterBase* c : validTargets)
	{
		if (c != primary)
		{
			action.Targets.Add(c);
		}
	}

	OutCandidates.Add(action);
}

void UUtilityAIComponent::EnumerateSingleTarget(UActiveSkillBase* Skill, ACharacterBase* Target,
                                                bool bAssumeMoved, TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !Target || !ownerCharacter || Target->IsDead()) return;

	const FVector aimLoc = Target->GetActorLocation();

	TArray<FAIAction> positions;
	GatherCastPositions(aimLoc, Skill->pickRange, bAssumeMoved, positions);

	for (const FAIAction& pos : positions)
	{
		//대표 대상에 대한 시야선 필요
		if (!UAINavigationHelper::HasLineOfSightFrom(ownerCharacter, pos.CastFrom, Target)) continue;

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

		BuildActionFromCast(Skill, pos, affected, aimLoc, OutCandidates);
	}
}

void UUtilityAIComponent::EnumerateMultiPick(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& PickTargets,
                                             bool bAssumeMoved, TArray<FAIAction>& OutCandidates) const
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
	TArray<FAIAction> positions;
	GatherCastPositions(centroid, Skill->pickRange, bAssumeMoved, positions, false);

	for (const FAIAction& pos : positions)
	{
		//시전 위치에서 사거리·시야선 확보된 픽 후보를 효용(회복량 또는 피해)으로 정렬
		TArray<TPair<ACharacterBase*, float>> cands;
		for (ACharacterBase* t : PickTargets)
		{
			if (!IsValid(t) || t->IsDead()) continue;
			if (Skill->pickRange > 0.f && FVector::Dist(pos.CastFrom, t->GetActorLocation()) > Skill->pickRange) continue;
			if (!UAINavigationHelper::HasLineOfSightFrom(ownerCharacter, pos.CastFrom, t)) continue;

			const FDamageResult pr = t->PreviewDamage(Skill, ownerCharacter, pos.CastFrom);
			const float hitP  = pr.HitChance / 100.f;

			//픽 효용 = 유효 회복(음수 데미지) 또는 기대 피해(양수) + rider 가치
			float value = 0.f;
			if (pr.NormalDamage < 0)
			{
				const int32 missing = t->GetMaxHp() - t->GetHp();
				const float healAmount = -static_cast<float>(pr.NormalDamage);
				if (missing > 0) value += hitP * FMath::Min(healAmount, static_cast<float>(missing));
			}
			else if (pr.NormalDamage > 0)
			{
				const float critP = pr.CritChance / 100.f;
				value += hitP * ((1.f - critP) * pr.NormalDamage + critP * pr.CritDamage);
			}
			float buffV = 0.f;
			float ailV = 0.f;
			value += ComputeRiderValue(Skill, t, hitP, buffV, ailV);

			//이득 없는 픽 제외
			if (value <= 0.f) continue;
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

		//조준점은 최고 효용 픽의 위치
		BuildActionFromCast(Skill, pos, affected, cands[0].Key->GetActorLocation(), OutCandidates);
	}
}

void UUtilityAIComponent::EnumerateGroundPoint(UActiveSkillBase* Skill, bool bAssumeMoved,
                                               TArray<FAIAction>& OutCandidates) const
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
		TArray<FAIAction> positions;
		GatherCastPositions(aim, Skill->pickRange, bAssumeMoved, positions);

		for (const FAIAction& pos : positions)
		{
			if (!HasLOSToLocation(pos.CastFrom, aim)) continue;

			TArray<ACharacterBase*> affected;
			CollectAreaAffected(Skill, aim, affected);
			BuildActionFromCast(Skill, pos, affected, aim, OutCandidates);
		}
	}
}

void UUtilityAIComponent::EnumerateSkillActions(UActiveSkillBase* Skill, bool bAssumeMoved,
                                                TArray<FAIAction>& OutCandidates) const
{
	if (!Skill || !ownerCharacter) return;

	//Self 대상: 회복·자기 버프 모두 제자리 후보로 충분, 이동 패스에서는 생략
	if (Skill->selectMode == ESelectMode::Self)
	{
		if (bAssumeMoved) return;
		TArray<ACharacterBase*> selfOnly;
		selfOnly.Add(ownerCharacter);

		//제자리 시전이므로 경로는 없고, 현재 서 있는 지형의 체류 가치만 반영
		FAIAction selfPos;
		selfPos.CastFrom = ownerCharacter->GetActorLocation();
		TArray<TObjectPtr<ATerrainBase>> here;
		UAINavigationHelper::GetTerrainsAt(GetWorld(), selfPos.CastFrom, here);
		selfPos.TerrainValueAtDest = SumTerrainStayValue(here);

		BuildActionFromCast(Skill, selfPos, selfOnly, selfPos.CastFrom, OutCandidates);
		return;
	}

	if (Skill->selectMode == ESelectMode::GroundPoint)
	{
		EnumerateGroundPoint(Skill, bAssumeMoved, OutCandidates);
		return;
	}

	//SinglePick, pickTeam으로 대상 집합 결정
	TArray<ACharacterBase*> pickTargets;
	ResolvePickTargets(Skill->pickTeam, pickTargets);
	if (pickTargets.Num() == 0) return;

	if (Skill->pickCount > 1)
	{
		EnumerateMultiPick(Skill, pickTargets, bAssumeMoved, OutCandidates);
	}
	else
	{
		for (ACharacterBase* t : pickTargets)
		{
			EnumerateSingleTarget(Skill, t, bAssumeMoved, OutCandidates);
		}
	}
}

void UUtilityAIComponent::EnumerateMoveActions(TArray<FAIAction>& OutCandidates) const
{
	if (!ownerCharacter) return;
	const float casterBudgetCm = ownerCharacter->GetCurrentMovingPoint() * 100.f;
	if (casterBudgetCm <= 0.f) return;

	const FVector casterLoc = ownerCharacter->GetActorLocation();

	//접근 축 기준 거리
	const float baseDist = DistToNearestPlayer(casterLoc);

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
			FTerrainPathInfo terrainInfo;
			if (!UAINavigationHelper::CanReach(ownerCharacter, sample, pathLenCm, &terrainInfo)) continue;

			FAIAction action;
			action.CastFrom = sample;
			action.PathLengthCm = pathLenCm;
			action.TerrainWeightedCostCm = terrainInfo.WeightedCostCm;
			action.TerrainValueAtDest = SumTerrainStayValue(terrainInfo.DestTerrains);
			action.TerrainPassThroughValue = SumTerrainPassValue(terrainInfo.PassedTerrains);
			action.ApproachDeltaCm = (baseDist >= 0.f) ? baseDist - DistToNearestPlayer(sample) : 0.f;
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
		//접근 축, 최근접 플레이어와의 거리 감소량 보상
		score += Action.ApproachDeltaCm * p->weightApproach;

		//지형 점수
		score += ScoreTerrain(Action);
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

		//버프/상태이상 rider 점수, 기대 HP 환산치라 기대 피해와 같은 저울
		score += Action.BuffValueTotal * p->weightBuffValue;
		score += Action.AilmentValueTotal * p->weightAilmentValue;

		//이동 거리 페널티
		score += Action.PathLengthCm * p->weightDistancePenalty;

		//지형 점수, 시전 위치의 체류 가치와 경로 통과 손익
		score += ScoreTerrain(Action);

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
						const FThreatProfile& tp = gm->GetThreatProfile(allyTarget);
						const float ownerEva = ownerCharacter ? ownerCharacter->GetEvasion() : 0.f;
						const float tHitP  = tp.Accuracy / FMath::Max(1.f, tp.Accuracy + ownerEva);
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

	//극단성 배율용 전장 평균 갱신
	UpdateBattlefieldAverages();

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
		//차감은 지형 배율을 반영한 실질 비용 기준, 실제 경로 길이가 아님
		StartMoveTo(best->CastFrom, best->TerrainWeightedCostCm);
		return;
	}

	if (best->PathLengthCm > 0.f)
	{
		//이동 후 공격 예약
		lastWasMove = false;
		pendingActionAfterMove = *best;
		bHasPendingAfterMove = true;
		//차감은 지형 배율을 반영한 실질 비용 기준, 실제 경로 길이가 아님
		StartMoveTo(best->CastFrom, best->TerrainWeightedCostCm);
		return;
	}

	//제자리 공격 후 다음 행동 예약
	lastWasMove = false;
	ExecuteAttackImmediate(*best);
	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	ScheduleStepNext();
}

void UUtilityAIComponent::ScheduleStepNext()
{
	if (bFastForward) { StepNext(); return; }
	GetWorld()->GetTimerManager().SetTimer(stepTimerHandle, this, &UUtilityAIComponent::StepNext, actionDelay, false);
}

void UUtilityAIComponent::SchedulePendingAfterMove()
{
	if (bFastForward) { ExecutePendingAfterMove(); return; }
	GetWorld()->GetTimerManager().SetTimer(stepTimerHandle, this, &UUtilityAIComponent::ExecutePendingAfterMove, actionDelay, false);
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

		//지원은 회복·버프·상태이상 가치, 그 외는 적군 기대 피해로 로그 표기
		if (Action.Type == EAIActionType::Support)
		{
			UE_LOG(LogTemp, Log, TEXT("[UtilityAI] %s 지원 스킬 사용: %s → 대표 %s, 대상 %d명 (회복 %.1f, 버프 %.1f, 상태이상 %.1f)"),
				*GetNameSafe(ownerCharacter), *Action.Skill->skillName.ToString(),
				*GetNameSafe(primary), targets.Num(),
				Action.HealExpectedTotal, Action.BuffValueTotal, Action.AilmentValueTotal);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[UtilityAI] %s 스킬 사용: %s → 대표 %s, 대상 %d명 (적군 기대피해 %.1f)"),
				*GetNameSafe(ownerCharacter), *Action.Skill->skillName.ToString(),
				*GetNameSafe(primary), targets.Num(), Action.EnemyExpectedDamage);
		}

		sc->DirectExecute(Action.Skill, targets, Action.AimPoint);
	}
}

void UUtilityAIComponent::StartMoveTo(const FVector& Dest, float MoveCostCm)
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
	ownerCharacter->ConsumeMovingPoint(MoveCostCm / 100.f);
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

	//예약된 이동 후 공격도 행동 간 딜레이 후 실행
	if (bHasPendingAfterMove)
	{
		bHasPendingAfterMove = false;
		SchedulePendingAfterMove();
		return;
	}

	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	ScheduleStepNext();
}

void UUtilityAIComponent::ExecutePendingAfterMove()
{
	ExecuteAttackImmediate(pendingActionAfterMove);
	if (ShouldStopAfterStep()) { FinishTurn(); return; }
	ScheduleStepNext();
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

		const FThreatProfile& prof = gm->GetThreatProfile(threat);
		const FVector threatLoc = threat->GetActorLocation();

		//사거리 밖 위협 제외
		if (prof.RangeCm > 0.f && FVector::Dist(threatLoc, AtLocation) > prof.RangeCm) continue;

		//시야선 없는 위협 제외
		FCollisionQueryParams params(SCENE_QUERY_STAT(AI_IncomingDangerLOS), false, threat);
		FHitResult hit;
		const bool bBlocked = world->LineTraceSingleByChannel(
			hit, threatLoc + eye, AtLocation + eye, ECC_Visibility, params);
		if (bBlocked && hit.GetActor() != ownerCharacter) continue;   //시야 차단

		//기대 피해 합산, 명중은 내 회피와의 비율로 환산
		const float ownerEva = ownerCharacter ? ownerCharacter->GetEvasion() : 0.f;
		const float hitP  = prof.Accuracy / FMath::Max(1.f, prof.Accuracy + ownerEva);
		const float critP = prof.CritChance / 100.f;
		total += hitP * ((1.f - critP) * prof.NormalDamage + critP * prof.CritDamage);
	}
	return total;
}

//최근접 생존 플레이어까지의 거리, 없으면 -1
float UUtilityAIComponent::DistToNearestPlayer(const FVector& From) const
{
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return -1.f;

	float best = -1.f;
	for (AAllyCharacterBase* a : gm->GetAllies())
	{
		if (!IsValid(a) || a->IsDead()) continue;
		const float d = FVector::Dist(a->GetActorLocation(), From);
		if (best < 0.f || d < best) best = d;
	}
	return best;
}

void UUtilityAIComponent::UpdateBattlefieldAverages()
{
	battlefieldAvg = FBattlefieldStatAverages();

	TArray<ACharacterBase*> all;
	CollectAllCharacters(all);
	if (all.Num() == 0) return;

	for (ACharacterBase* c : all)
	{
		battlefieldAvg.hp += c->GetMaxHp();
		battlefieldAvg.atk += c->GetAtk();
		battlefieldAvg.def += c->GetDef();
		battlefieldAvg.speed += c->GetSpeed();
		battlefieldAvg.skill += c->GetSkill();
		battlefieldAvg.movingPoint += c->GetMovingPoint();
		battlefieldAvg.damageReduction += c->GetDamageReduction();
		battlefieldAvg.damageAmplification += c->GetDamageAmplification();
		battlefieldAvg.penetration += c->GetPenetration();
		battlefieldAvg.accuracy += c->GetAccuracy();
		battlefieldAvg.evasion += c->GetEvasion();
		battlefieldAvg.critical += c->GetCritical();
		battlefieldAvg.criticalDamage += c->GetCriticalDamage();
	}

	const float n = static_cast<float>(all.Num());
	battlefieldAvg.hp /= n;
	battlefieldAvg.atk /= n;
	battlefieldAvg.def /= n;
	battlefieldAvg.speed /= n;
	battlefieldAvg.skill /= n;
	battlefieldAvg.movingPoint /= n;
	battlefieldAvg.damageReduction /= n;
	battlefieldAvg.damageAmplification /= n;
	battlefieldAvg.penetration /= n;
	battlefieldAvg.accuracy /= n;
	battlefieldAvg.evasion /= n;
	battlefieldAvg.critical /= n;
	battlefieldAvg.criticalDamage /= n;
}

float UUtilityAIComponent::ComputeExpectedOutput(const ACharacterBase* Target) const
{
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm || !Target) return 0.f;

	//상대 진영 평균 회피 기준 기대 산출, 특정 대상이 없는 일반 위협 지표
	const FThreatProfile& prof = gm->GetThreatProfile(Target);
	const float hitP  = prof.Accuracy / FMath::Max(1.f, prof.Accuracy + battlefieldAvg.evasion);
	const float critP = prof.CritChance / 100.f;
	return hitP * ((1.f - critP) * prof.NormalDamage + critP * prof.CritDamage);
}

float UUtilityAIComponent::ComputeIncomingFor(const ACharacterBase* Target, float& OutRawDamage) const
{
	OutRawDamage = 0.f;
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm || !Target) return 0.f;

	const FVector targetLoc = Target->GetActorLocation();

	//대상의 상대 진영이 위협
	TArray<ACharacterBase*> threats;
	if (Target->IsAlly())
	{
		for (AEnemyBase* e : gm->GetEnemies())
		{
			if (IsValid(e) && !e->IsDead()) threats.Add(e);
		}
	}
	else
	{
		for (AAllyCharacterBase* a : gm->GetAllies())
		{
			if (IsValid(a) && !a->IsDead()) threats.Add(a);
		}
	}

	float total = 0.f;
	for (ACharacterBase* threat : threats)
	{
		const FThreatProfile& prof = gm->GetThreatProfile(threat);

		//사거리 밖 위협 제외
		if (prof.RangeCm > 0.f && FVector::Dist(threat->GetActorLocation(), targetLoc) > prof.RangeCm) continue;

		const float hitP  = prof.Accuracy / FMath::Max(1.f, prof.Accuracy + Target->GetEvasion());
		const float critP = prof.CritChance / 100.f;
		const float raw = (1.f - critP) * prof.NormalDamage + critP * prof.CritDamage;
		OutRawDamage += raw;
		total += hitP * raw;
	}
	return total;
}

float UUtilityAIComponent::OpposingTeamAverageDef(const ACharacterBase* Target) const
{
	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm || !Target) return 0.f;

	float sum = 0.f;
	int32 count = 0;
	if (Target->IsAlly())
	{
		for (AEnemyBase* e : gm->GetEnemies())
		{
			if (IsValid(e) && !e->IsDead()) { sum += e->GetDef(); ++count; }
		}
	}
	else
	{
		for (AAllyCharacterBase* a : gm->GetAllies())
		{
			if (IsValid(a) && !a->IsDead()) { sum += a->GetDef(); ++count; }
		}
	}
	return count > 0 ? sum / count : 0.f;
}

float UUtilityAIComponent::StatLeverage(EAIBuffStat Stat, const ACharacterBase* Target) const
{
	if (!Target) return 0.f;

	//플랫 스탯은 상수, 산식 유도값
	switch (Stat)
	{
	case EAIBuffStat::Hp:          return 0.8f;
	case EAIBuffStat::Atk:         return 0.8f;
	case EAIBuffStat::Def:         return 0.8f;
	case EAIBuffStat::MovingPoint: return 0.25f;
	default: break;
	}

	UWorld* world = GetWorld();
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gm) return 0.f;

	//퍼센트 스탯은 앵커의 1%, 프로파일 NormalDamage는 방어 적용 전 원시 일격 피해
	const FThreatProfile& prof = gm->GetThreatProfile(Target);
	const float hitP  = prof.Accuracy / FMath::Max(1.f, prof.Accuracy + battlefieldAvg.evasion);
	const float critP = prof.CritChance / 100.f;
	const float strikeDmg = prof.NormalDamage;

	//기대 일격 = D·(1 + p·(m−1)), m은 대상의 치명타 배율
	const float critMul = Target->GetCriticalDamage();
	const float critFactor = 1.f + critP * (critMul - 1.f);

	//비율 명중의 기울기, 명중 1p당 eva/(acc+eva)^2 · 회피 1p당 acc/(acc+eva)^2
	const float accSum = FMath::Max(1.f, Target->GetAccuracy() + battlefieldAvg.evasion);
	const float accSlope = battlefieldAvg.evasion / (accSum * accSum);
	const float evaSum = FMath::Max(1.f, battlefieldAvg.accuracy + Target->GetEvasion());
	const float evaSlope = battlefieldAvg.accuracy / (evaSum * evaSum);

	switch (Stat)
	{
	case EAIBuffStat::Accuracy:
		return accSlope * strikeDmg * critFactor;
	case EAIBuffStat::Critical:
		return 0.01f * hitP * strikeDmg * (critMul - 1.f);
	//퍼센트가 아닌 배율 단위라 1.0(=피해 100%p)당 가치
	case EAIBuffStat::CriticalDamage:
		return hitP * strikeDmg * critP;
	case EAIBuffStat::DamageAmplification:
		return 0.01f * hitP * critFactor * strikeDmg;
	case EAIBuffStat::Penetration:
		return 0.01f * hitP * critFactor * OpposingTeamAverageDef(Target);
	case EAIBuffStat::Evasion:
	{
		//회피 1p의 가치는 명중률 미적용 원시 피격 피해 × 피격 확률 감소분
		float rawIncoming = 0.f;
		ComputeIncomingFor(Target, rawIncoming);
		return evaSlope * rawIncoming;
	}
	case EAIBuffStat::DamageReduction:
	{
		float rawIncoming = 0.f;
		return 0.01f * ComputeIncomingFor(Target, rawIncoming);
	}
	//파생 스탯은 SetDefaultStats 공식(skill×1.2→accuracy, skill×0.5→critical, speed×1.2→evasion)과 연동
	case EAIBuffStat::Skill:
		return 1.2f * StatLeverage(EAIBuffStat::Accuracy, Target)
		     + 0.5f * StatLeverage(EAIBuffStat::Critical, Target);
	case EAIBuffStat::Speed:
		return 1.2f * StatLeverage(EAIBuffStat::Evasion, Target);
	default:
		return 0.f;
	}
}

float UUtilityAIComponent::ExtremenessMultiplier(float TargetStat, float BattlefieldAvg, bool bSameTeam) const
{
	if (BattlefieldAvg <= 0.f) return 1.f;

	const UAIPersonalityData* p = personalityData
		? personalityData.Get()
		: GetDefault<UAIPersonalityData>();

	//e>0이면 전장 평균보다 높은 스탯, (팀, 부호)로 성향 가중치 선택
	const float e = TargetStat / BattlefieldAvg - 1.f;
	float w;
	if (bSameTeam)
	{
		w = (e >= 0.f) ? p->weightPreferHighStatAlly : p->weightPreferLowStatAlly;
	}
	else
	{
		w = (e >= 0.f) ? p->weightPreferHighStatEnemy : p->weightPreferLowStatEnemy;
	}
	//기피(음수 w)가 커도 가치 부호는 뒤집지 않음
	return FMath::Max(0.f, 1.f + w * FMath::Abs(e));
}

float UUtilityAIComponent::BuffValueForTarget(const UBuffBase* BuffCDO, const ACharacterBase* Target) const
{
	if (!BuffCDO || !Target || !ownerCharacter) return 0.f;

	const bool bSameTeam = (Target->IsAlly() == ownerCharacter->IsAlly());

	//스탯별 델타 × 레버리지 × 극단성 배율, 델타 0이면 항 소거
	auto term = [&](float delta, EAIBuffStat stat, float targetStat, float avg) -> float
	{
		if (delta == 0.f) return 0.f;
		return delta * StatLeverage(stat, Target) * ExtremenessMultiplier(targetStat, avg, bSameTeam);
	};

	//스탯 외 효과의 수동 환산치, 극단성 미적용
	float value = BuffCDO->aiBaseValue;
	value += term(BuffCDO->hp, EAIBuffStat::Hp, Target->GetMaxHp(), battlefieldAvg.hp);
	value += term(BuffCDO->atk, EAIBuffStat::Atk, Target->GetAtk(), battlefieldAvg.atk);
	value += term(BuffCDO->def, EAIBuffStat::Def, Target->GetDef(), battlefieldAvg.def);
	value += term(BuffCDO->speed, EAIBuffStat::Speed, Target->GetSpeed(), battlefieldAvg.speed);
	value += term(BuffCDO->skill, EAIBuffStat::Skill, Target->GetSkill(), battlefieldAvg.skill);
	value += term(BuffCDO->movingPoint, EAIBuffStat::MovingPoint, Target->GetMovingPoint(), battlefieldAvg.movingPoint);
	value += term(BuffCDO->damageReduction, EAIBuffStat::DamageReduction, Target->GetDamageReduction(), battlefieldAvg.damageReduction);
	value += term(BuffCDO->damageAmplification, EAIBuffStat::DamageAmplification, Target->GetDamageAmplification(), battlefieldAvg.damageAmplification);
	value += term(BuffCDO->penetration, EAIBuffStat::Penetration, Target->GetPenetration(), battlefieldAvg.penetration);
	value += term(BuffCDO->accuracy, EAIBuffStat::Accuracy, Target->GetAccuracy(), battlefieldAvg.accuracy);
	value += term(BuffCDO->evasion, EAIBuffStat::Evasion, Target->GetEvasion(), battlefieldAvg.evasion);
	value += term(BuffCDO->critical, EAIBuffStat::Critical, Target->GetCritical(), battlefieldAvg.critical);
	value += term(BuffCDO->criticalDamage, EAIBuffStat::CriticalDamage, Target->GetCriticalDamage(), battlefieldAvg.criticalDamage);
	//mentality/sight/actionPoint는 레버리지 미정의로 평가 제외

	return value;
}

float UUtilityAIComponent::AilmentValueForTarget(const UAilmentBase* AilmentCDO, const ACharacterBase* Target) const
{
	if (!AilmentCDO || !Target) return 0.f;

	//합산형: DoT 성분 + 제어 성분 × 대상 턴당 기대 대미지, 성분 0이면 자동 소거
	return AilmentCDO->aiDamagePerTurn
	     + AilmentCDO->aiControlCoefficient * ComputeExpectedOutput(Target);
}

//행동 종류 ASCII 라벨, 디버그 폰트 한글 미지원 대응
static const TCHAR* ActionTypeStr(EAIActionType Type)
{
	switch (Type)
	{
		case EAIActionType::Attack:  return TEXT("ATK");
		case EAIActionType::Support: return TEXT("SUP");
		case EAIActionType::Move:    return TEXT("MOV");
		case EAIActionType::Wait:    return TEXT("WAIT");
	}
	return TEXT("?");
}

void UUtilityAIComponent::DrawDebugCandidates(const TArray<FAIAction>& Candidates,
                                              const TArray<float>& Scores,
                                              const FAIAction* Best) const
{
	if (CVarUtilityAIDebug.GetValueOnGameThread() == 0) return;
	UWorld* world = GetWorld();
	if (!world || Candidates.Num() != Scores.Num()) return;

	//DrawDebugString은 HUD 텍스트라 카메라 분리(eject) 시 안 보임, 로그로도 점수 상위 후보 덤프
	{
		TArray<int32> order;
		order.Reserve(Candidates.Num());
		for (int32 i = 0; i < Candidates.Num(); ++i)
		{
			order.Add(i);
		}
		order.Sort([&Scores](int32 A, int32 B) { return Scores[A] > Scores[B]; });

		//레벨 1은 상위 15개, 레벨 2는 전체
		const int32 logN = (CVarUtilityAIDebug.GetValueOnGameThread() >= 2)
			? order.Num() : FMath::Min(order.Num(), 15);
		UE_LOG(LogTemp, Log, TEXT("[UtilityAI] %s 후보 %d개 (상위 %d개 표시)"),
			*GetNameSafe(ownerCharacter), Candidates.Num(), logN);

		for (int32 k = 0; k < logN; ++k)
		{
			const FAIAction& c = Candidates[order[k]];
			const float s = Scores[order[k]];
			const bool bIsBest = (Best && &c == Best);

			UE_LOG(LogTemp, Log, TEXT("  %s%s %s>%s  %.1f | dmg %.1f ff %.1f heal %.1f buff %.1f ail %.1f dan %.1f app %.0f path %.0f"),
				bIsBest ? TEXT(">> ") : TEXT("   "), ActionTypeStr(c.Type),
				c.Skill ? *c.Skill->GetClass()->GetName() : TEXT("-"),
				*GetNameSafe(c.GetPrimaryTarget()), s,
				c.EnemyExpectedDamage, c.AllyExpectedDamage, c.HealExpectedTotal,
				c.BuffValueTotal, c.AilmentValueTotal, c.IncomingDangerExpected, c.ApproachDeltaCm, c.PathLengthCm);
		}
	}

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

		//행동 종류 문자열, 디버그 폰트가 한글 미지원이라 ASCII 사용
		const TCHAR* typeStr = TEXT("?");
		switch (c.Type)
		{
			case EAIActionType::Attack:  typeStr = TEXT("ATK");  break;
			case EAIActionType::Support: typeStr = TEXT("SUP");  break;
			case EAIActionType::Move:    typeStr = TEXT("MOV");  break;
			case EAIActionType::Wait:    typeStr = TEXT("WAIT"); break;
		}

		//스킬 후보는 스킬 클래스와 대표 대상까지 표기
		FString label;
		if (c.Skill)
		{
			label = FString::Printf(TEXT("%s%s %s>%s  %.1f"),
				bIsBest ? TEXT(">> ") : TEXT(""), typeStr,
				*c.Skill->GetClass()->GetName(), *GetNameSafe(c.GetPrimaryTarget()), score);
		}
		else
		{
			label = FString::Printf(TEXT("%s%s  %.1f"),
				bIsBest ? TEXT(">> ") : TEXT(""), typeStr, score);
		}
		DrawDebugString(world, c.CastFrom + textOffset, label,
			nullptr, color, duration, false, bIsBest ? 2.2f : 1.4f);

		//점수 구성 요소 상세, 최고점은 항상·나머지는 레벨 2에서 표기
		if (bIsBest || CVarUtilityAIDebug.GetValueOnGameThread() >= 2)
		{
			const FString detail = FString::Printf(
				TEXT("dmg %.1f ff %.1f heal %.1f buff %.1f ail %.1f dan %.1f app %.0f path %.0f"),
				c.EnemyExpectedDamage, c.AllyExpectedDamage, c.HealExpectedTotal,
				c.BuffValueTotal, c.AilmentValueTotal, c.IncomingDangerExpected, c.ApproachDeltaCm, c.PathLengthCm);
			DrawDebugString(world, c.CastFrom + textOffset - FVector(0.f, 0.f, 22.f), detail,
				nullptr, color, duration, false, bIsBest ? 1.4f : 1.0f);
		}

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
