//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/RifleMan/Random/Top/Skill/RiflePackLeader.h"
#include "Characters/CharacterBase.h"
#include "Characters/AllyCharacterBase.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"
#include "GameMode/BattleGameMode.h"

URiflePackLeader::URiflePackLeader()
{
	skillName = NSLOCTEXT("Skill", "RiflePackLeader_Name", "팩 리더");
	skillDescription = NSLOCTEXT("Skill", "RiflePackLeader_Description", "자신이 버프로 얻은 능력치를 주변 8m 내의 아군에게 함께 제공한다");
	skillGrade = EUpgradeGrade::Top;

	//누가 이동을 마칠 때마다 주변 인원과 버프 합계를 다시 평가
	passiveType = EPassiveType::Conditional;
	reactiveType = EReactiveType::None;
	conditionType = EConditionalType::MoveComplete;

	//적용 대상과 적용량을 인스턴스가 추적하므로 중복 획득 불가
	bAllowDuplicate = false;
}

void URiflePackLeader::Execute_Conditional()
{
	ACharacterBase* o = owner.Get();
	if (!o) return;

	const UWorld* world = o->GetWorld();
	ABattleGameMode* gameMode = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	if (!gameMode) return;

	//나눠주는 양이 매번 달라지므로 범위 유지 여부와 무관하게 전원 회수 후 다시 부여한다
	//회수는 반드시 필드 갱신 전에 끝나야 걸어준 값과 되돌리는 값이 일치한다
	for (const TWeakObjectPtr<ACharacterBase>& weakAlly : sharedAllies)
	{
		ACharacterBase* previous = weakAlly.Get();
		//죽었거나 파괴된 대상은 되돌릴 스탯도 없다
		if (IsValid(previous) && !previous->IsDead())
		{
			previous->ApplyPassiveStatDelta(this, -1);
		}
	}
	sharedAllies.Empty();

	//나눠줄 보너스가 없으면 빈 효과만 아군에게 표시되므로 부여 자체를 생략
	if (!SnapshotBuffTotals(o)) return;

	for (AAllyCharacterBase* ally : gameMode->GetAllies())
	{
		if (!IsValid(ally) || ally == o || ally->IsDead()) continue;
		if (FVector::Dist2D(o->GetActorLocation(), ally->GetActorLocation()) > auraRange) continue;

		ally->ApplyPassiveStatDelta(this, +1);
		sharedAllies.Add(ally);
	}
}

bool URiflePackLeader::SnapshotBuffTotals(const ACharacterBase* holder)
{
	hp = 0;
	atk = 0;
	speed = 0;
	skill = 0;
	def = 0;
	movingPoint = 0.f;
	mentality = 0;
	actionPoint = 0;
	damageReduction = 0;
	damageAmplification = 0;
	penetration = 0;
	sight = 0.f;
	accuracy = 0.f;
	evasion = 0.f;
	critical = 0.f;
	criticalDamage = 0.f;

	const UBuffComponent* buffComp = holder ? holder->GetBuffComponent() : nullptr;
	if (!buffComp) return false;

	for (const UBuffBase* buff : buffComp->GetActiveBuffs())
	{
		if (!buff) continue;

		hp += buff->hp;
		atk += buff->atk;
		speed += buff->speed;
		skill += buff->skill;
		def += buff->def;
		movingPoint += buff->movingPoint;
		mentality += buff->mentality;
		actionPoint += buff->actionPoint;
		damageReduction += buff->damageReduction;
		damageAmplification += buff->damageAmplification;
		penetration += buff->penetration;
		sight += buff->sight;
		accuracy += buff->accuracy;
		evasion += buff->evasion;
		critical += buff->critical;
		criticalDamage += buff->criticalDamage;
	}

	//디버프도 UBuffBase라 음수가 섞여 들어온다, 합계에서 음수는 잘라 이득만 넘긴다
	hp = FMath::Max(0, hp);
	atk = FMath::Max(0, atk);
	speed = FMath::Max(0, speed);
	skill = FMath::Max(0, skill);
	def = FMath::Max(0, def);
	movingPoint = FMath::Max(0.f, movingPoint);
	mentality = FMath::Max(0, mentality);
	actionPoint = FMath::Max(0, actionPoint);
	damageReduction = FMath::Max(0, damageReduction);
	damageAmplification = FMath::Max(0, damageAmplification);
	penetration = FMath::Max(0, penetration);
	sight = FMath::Max(0.f, sight);
	accuracy = FMath::Max(0.f, accuracy);
	evasion = FMath::Max(0.f, evasion);
	critical = FMath::Max(0.f, critical);
	criticalDamage = FMath::Max(0.f, criticalDamage);

	return hp != 0 || atk != 0 || speed != 0 || skill != 0 || def != 0
		|| movingPoint != 0.f || mentality != 0 || actionPoint != 0
		|| damageReduction != 0 || damageAmplification != 0 || penetration != 0
		|| sight != 0.f || accuracy != 0.f || evasion != 0.f
		|| critical != 0.f || criticalDamage != 0.f;
}
