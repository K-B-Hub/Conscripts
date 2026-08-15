//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Stress/Ailment/ConfusionAilment.h"
#include "Characters/AllyCharacterBase.h"
#include "Characters/EnemyBase.h"
#include "GameMode/BattleGameMode.h"
#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "AI/AINavigationHelper.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

UConfusionAilment::UConfusionAilment()
{
	ailmentName = NSLOCTEXT("Ailment", "Confusion_Name", "PTSD");
	ailmentDescription = NSLOCTEXT("Ailment", "Confusion_Description", "적과 아군을 구분하지 못한다. 매 턴 무작위 공격 스킬을 닿는 범위의 무작위 대상에게 사용한 뒤 턴이 종료된다");

	ailmentTurn = 2;
	isStart = true;
	isEnd = false;
	isStackable = false;
}

void UConfusionAilment::Execute(ACharacterBase* affected)
{
	Super::Execute(affected);

	//스트레스 전용 상태이상, 아군 외 대상은 행동 없이 무시
	AAllyCharacterBase* ally = Cast<AAllyCharacterBase>(affected);
	if (!ally) return;

	//시전이 끝날 때까지 플레이어가 끼어들지 못하도록 턴 점유 선언
	ally->MarkAilmentDrivenTurn();

	UActiveSkillBase* skill = nullptr;
	ACharacterBase* target = nullptr;
	FVector castFrom = FVector::ZeroVector;
	if (!PickRandomSkillAndTarget(ally, skill, target, castFrom))
	{
		//시전할 게 없으면 행동 없이 턴 종료, 즉시 넘기지 않도록 딜레이
		ScheduleWithPacing(ally, [](ACharacterBase* character) { character->RequestEndTurn(); });
		return;
	}

	confusedAlly = ally;
	pendingSkill = skill;
	pendingTarget = target;

	//제자리에서 사거리가 닿으면 이동 없이 시전, 카메라가 시전자를 잡을 시간만 확보
	if (castFrom.Equals(ally->GetActorLocation()))
	{
		ScheduleWithPacing(ally, [this](ACharacterBase*) { CastAndWaitCommit(); });
		return;
	}

	UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(
		ally->GetWorld(), ally->GetActorLocation(), castFrom);
	if (!path || path->PathPoints.Num() < 2)
	{
		ScheduleWithPacing(ally, [](ACharacterBase* character) { character->RequestEndTurn(); });
		return;
	}

	//시전 지점 도착 후 시전, 재발동 대비 재바인딩
	ally->OnMovementCompleted.RemoveAll(this);
	ally->OnMovementCompleted.AddUObject(this, &UConfusionAilment::OnApproachCompleted);

	//카메라가 시전자를 잡을 시간을 준 뒤 접근 시작
	const TArray<FVector> pathPoints = path->PathPoints;
	ScheduleWithPacing(ally, [pathPoints](ACharacterBase* character)
	{
		if (AAllyCharacterBase* confused = Cast<AAllyCharacterBase>(character))
		{
			confused->MoveAlongPath(pathPoints);
		}
	});
}

bool UConfusionAilment::PickRandomSkillAndTarget(AAllyCharacterBase* ally, UActiveSkillBase*& outSkill,
	ACharacterBase*& outTarget, FVector& outCastFrom) const
{
	USkillComponent* skillComp = ally->GetSkillComponent();
	ABattleGameMode* gm = ally->GetWorld()->GetAuthGameMode<ABattleGameMode>();
	if (!skillComp || !gm) return false;

	//때리는 스킬만 후보, 회복·버프·상태이상 스킬은 제외
	TArray<UActiveSkillBase*> skills;
	for (UActiveSkillBase* skill : skillComp->GetActiveSkills())
	{
		if (!skill || !skill->CanExecute()) continue;
		if (skill->skillType != ESkillType::Melee
			&& skill->skillType != ESkillType::Ranged
			&& skill->skillType != ESkillType::Throw) continue;

		skills.Add(skill);
	}
	if (skills.Num() == 0) return false;

	//피아 구분 없는 대상 풀, 본인은 제외
	TArray<ACharacterBase*> characters;
	for (AAllyCharacterBase* other : gm->GetAllies())
	{
		if (other && other != ally && !other->IsDead()) characters.Add(other);
	}
	for (AEnemyBase* enemy : gm->GetEnemies())
	{
		if (enemy && !enemy->IsDead()) characters.Add(enemy);
	}
	if (characters.Num() == 0) return false;

	//스킬을 무작위로 뽑되 접근 가능한 대상이 없으면 다음 스킬로, 못 쓰는 스킬을 뽑아 턴을 날리지 않도록
	while (skills.Num() > 0)
	{
		const int32 skillIndex = FMath::RandRange(0, skills.Num() - 1);
		UActiveSkillBase* skill = skills[skillIndex];
		skills.RemoveAtSwap(skillIndex);

		//사거리와 잔여 이동력으로 닿는 대상만 수집, 시전 지점은 대상마다 다름
		TArray<TPair<ACharacterBase*, FVector>> reachables;
		for (ACharacterBase* character : characters)
		{
			FVector castFrom = FVector::ZeroVector;
			if (FindCastPosition(ally, skill, character, castFrom))
			{
				reachables.Emplace(character, castFrom);
			}
		}
		if (reachables.Num() == 0) continue;

		const TPair<ACharacterBase*, FVector>& picked = reachables[FMath::RandRange(0, reachables.Num() - 1)];
		outSkill = skill;
		outTarget = picked.Key;
		outCastFrom = picked.Value;
		return true;
	}

	return false;
}

bool UConfusionAilment::FindCastPosition(AAllyCharacterBase* ally, const UActiveSkillBase* skill,
	const ACharacterBase* target, FVector& outCastFrom) const
{
	const FVector allyLoc = ally->GetActorLocation();
	const FVector targetLoc = target->GetActorLocation();
	//점프류는 사거리가 곧 잔여 이동력이므로 실효 사거리 사용
	const float range = skill->GetEffectivePickRange();

	auto DistToTarget2D = [&targetLoc](const FVector& point)
	{
		return FVector2D(targetLoc.X - point.X, targetLoc.Y - point.Y).Size();
	};

	//제자리에서 닿으면 이동 불필요
	if ((range <= 0.f || DistToTarget2D(allyLoc) <= range)
		&& UAINavigationHelper::HasLineOfSightFrom(ally, allyLoc, target))
	{
		outCastFrom = allyLoc;
		return true;
	}
	//사거리 제한이 없는데 시야선이 막혔으면 접근해도 의미 없음
	if (range <= 0.f) return false;

	UNavigationPath* path = UNavigationSystemV1::FindPathToLocationSynchronously(
		ally->GetWorld(), allyLoc, targetLoc);
	if (!path || path->PathPoints.Num() < 2) return false;

	//경로를 따라가며 사거리에 진입하는 첫 지점을 시전 지점으로, 필요한 이동을 최소화
	for (int32 i = 0; i + 1 < path->PathPoints.Num(); ++i)
	{
		const FVector& segStart = path->PathPoints[i];
		const FVector& segEnd = path->PathPoints[i + 1];
		const float distStart = DistToTarget2D(segStart);
		const float distEnd = DistToTarget2D(segEnd);

		//사거리 경계를 넘는 세그먼트가 아니면 통과
		if (distStart < range || distEnd >= range) continue;

		const float alpha = (distStart - range) / (distStart - distEnd);
		const FVector candidate = FMath::Lerp(segStart, segEnd, alpha);

		if (!UAINavigationHelper::HasLineOfSightFrom(ally, candidate, target)) continue;

		//잔여 이동력으로 도달 가능해야 접근 가능한 대상
		float pathLenCm = 0.f;
		if (!UAINavigationHelper::CanReach(ally, candidate, pathLenCm)) continue;

		outCastFrom = candidate;
		return true;
	}

	return false;
}

void UConfusionAilment::OnApproachCompleted()
{
	AAllyCharacterBase* ally = confusedAlly.Get();
	if (!ally) return;

	ally->OnMovementCompleted.RemoveAll(this);

	//도착 직후 바로 시전하지 않고 조준하는 간격을 둠, 적 AI의 이동 후 행동 딜레이와 동일
	ScheduleWithPacing(ally, [this](ACharacterBase*) { CastAndWaitCommit(); });
}

void UConfusionAilment::CastAndWaitCommit()
{
	AAllyCharacterBase* ally = confusedAlly.Get();
	if (!ally) return;

	ACharacterBase* target = pendingTarget.Get();
	USkillComponent* skillComp = ally->GetSkillComponent();
	if (!target || !pendingSkill || !skillComp)
	{
		ScheduleWithPacing(ally, [](ACharacterBase* character) { character->RequestEndTurn(); });
		return;
	}

	//인디케이터 없는 직접 시전, 명중 판정·효과 적용은 커밋 시점에 처리됨
	skillComp->DirectExecute(pendingSkill, { target }, target->GetActorLocation());

	//커밋은 몽타주 비동기라 완료 폴링 후 턴 종료
	ally->GetWorldTimerManager().SetTimer(commitWaitHandle, this, &UConfusionAilment::PollCommitFinished, 0.1f, true);
}

void UConfusionAilment::PollCommitFinished()
{
	AAllyCharacterBase* ally = confusedAlly.Get();
	if (!ally)
	{
		//대상 소실, 폴링 중단
		if (UWorld* world = GetWorld())
		{
			world->GetTimerManager().ClearTimer(commitWaitHandle);
		}
		return;
	}

	USkillComponent* skillComp = ally->GetSkillComponent();
	if (skillComp && skillComp->HasPendingExecution()) return;

	ally->GetWorldTimerManager().ClearTimer(commitWaitHandle);

	//피격 연출이 끝나고 결과를 볼 시간을 준 뒤 턴 종료
	ScheduleWithPacing(ally, [](ACharacterBase* character) { character->RequestEndTurn(); });
}