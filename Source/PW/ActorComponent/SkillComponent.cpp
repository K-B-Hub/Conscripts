//Fill out your copyright notice in the Description page of Project Settings.

#include "ActorComponent/SkillComponent.h"
#include "Object/Skill/SkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Object/Skill/AreaShapeLibrary.h"
#include "Actors/SkillRangeIndicator.h"
#include "Actors/AttackRangeIndicator.h"
#include "Actors/ProjectileBase.h"
#include "Characters/CharacterBase.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "GameMode/BattleGameMode.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "AnimInstance/AnimInstanceBase.h"
#include "AnimInstance/AnimNotify_SkillImpact.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USkillComponent::SetOwner(ACharacterBase* owner)
{
	if (owner->IsValidLowLevel())
	{
		ownerCharacter = owner;
	}
}

ACharacterBase* USkillComponent::GetOwner()
{
		return ownerCharacter;
}

void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	//CDO에서 실제 객체로 재등록
	ownerCharacter = Cast<ACharacterBase>(UActorComponent::GetOwner());
}

void USkillComponent::AddSkill(TSubclassOf<USkillBase> skillClass)
{
	if (!skillClass || !ownerCharacter) return;

	USkillBase* instance = NewObject<USkillBase>(this, skillClass);
	if (!instance) return;

	instance->SetOwner(ownerCharacter);
	skills.Add(instance);

	if (UActiveSkillBase* ActiveSkill = Cast<UActiveSkillBase>(instance))
	{
		ActiveSkill->SetCalcedStats();
	}
}

bool USkillComponent::HasSkillClass(TSubclassOf<USkillBase> skillClass) const
{
	if (!skillClass) return false;

	for (const USkillBase* skill : skills)
	{
		if (skill && skill->GetClass() == skillClass)
		{
			return true;
		}
	}
	return false;
}

void USkillComponent::RemoveSkill(USkillBase* Skill)
{
	if (!Skill) return;

	if (currentSkill == Skill)
	{
		DeactivateSkill();
	}
	skills.Remove(Skill);
}

TArray<UActiveSkillBase*> USkillComponent::GetActiveSkills() const
{
	TArray<UActiveSkillBase*> ActiveSkills;
	for (USkillBase* Skill : skills)
	{
		if (UActiveSkillBase* Active = Cast<UActiveSkillBase>(Skill))
		{
			ActiveSkills.Add(Active);
		}
	}
	return ActiveSkills;
}

void USkillComponent::ActivateSkill(UActiveSkillBase* Skill)
{
	if (!Skill || !Skill->CanExecute()) return;

	//이전 스킬 커밋 대기 중 신규 활성화 차단
	if (HasPendingExecution()) return;

	if (currentSkill)
	{
		DeactivateSkill();
	}

	currentSkill = Skill;
	SpawnIndicators(Skill);

	UE_LOG(LogTemp, Log, TEXT("[SkillComponent] 스킬 활성화: %s"), *Skill->skillName.ToString());
}

void USkillComponent::DeactivateSkill()
{
	if (!currentSkill) return;

	UE_LOG(LogTemp, Log, TEXT("[SkillComponent] 스킬 비활성화: %s"), *currentSkill->skillName.ToString());

	//인디케이터와 대상 목록 정리
	DestroyIndicators();
	ClearCurrentTargets();
	ClearAccumulatedTargets();
	currentSkill = nullptr;

	//인디케이터가 자동이동 예상으로 켜둔 투기적 이동 상태를 실제 이동 여부로 복구
	//최대치 비교는 달리기 보너스·이동력 버프에 흔들리므로 소모 플래그를 사용
	if (ownerCharacter)
	{
		ownerCharacter->OnMoveStateChanged(ownerCharacter->HasConsumedMovingPoint());
	}
}

void USkillComponent::AddTarget(ACharacterBase* Target)
{
	if (Target)
	{
		currentTargets.AddUnique(Target);
	}
}

void USkillComponent::RemoveTarget(ACharacterBase* Target)
{
	if (Target)
	{
		currentTargets.Remove(Target);
	}
}

void USkillComponent::ClearCurrentTargets()
{
	currentTargets.Empty();
}

void USkillComponent::AccumulateCurrentTargets()
{
	accumulatedTargets.Append(currentTargets);
}

void USkillComponent::ClearAccumulatedTargets()
{
	accumulatedTargets.Empty();
}

void USkillComponent::CalcSkillStats()
{
	for (USkillBase* Skill : skills)
	{
		if (UActiveSkillBase* Active = Cast<UActiveSkillBase>(Skill))
		{
			Active->SetCalcedStats();
		}
	}
}

void USkillComponent::RecalculatePending(ACharacterBase* Target)
{
	RecalculatePending(currentSkill, Target);
}

void USkillComponent::RecalculatePending(UActiveSkillBase* Skill, ACharacterBase* Target)
{
	if (!Target || !Skill || !ownerCharacter) return;

	//스킬 계산값을 로컬 값으로 복사
	float dmg  = Skill->calcDamage;
	int32 amp  = Skill->calcDamageAmplfication;
	int32 pen  = Skill->calcPenetration;
	float acc  = Skill->calcAccuracy;
	float crit = Skill->calcCritical;
	//배율은 공격자 스탯에서 출발, 패시브 보너스가 여기에 가산됨
	float critDmg = ownerCharacter->GetCriticalDamage();

	//공격자 패시브 보너스 반영
	if (UPassiveSkillComponent* PSC = ownerCharacter->GetPassiveSkillComponent())
	{
		PSC->DispatchBeforeDamageCalc(Target, Skill->skillType, Skill->damageType,
			ownerCharacter->GetActorLocation(),
			dmg, amp, pen, acc, crit, critDmg);
	}

	//패시브는 피해를 가산만 하므로 그 증가분을 상한에도 그대로 얹는다
	//디스패치를 다시 돌리면 조건부 패시브가 두 번 평가되므로 차분을 쓴다
	const float passiveBonus = dmg - Skill->calcDamage;
	const float dmgMax = Skill->calcDamageMax + passiveBonus;

	Target->CalculateDamage(
		dmg,
		dmgMax,
		acc,
		crit,
		critDmg,
		amp,
		pen,
		Skill->skillType,
		ownerCharacter
	);
}

bool USkillComponent::DirectExecute(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets, const FVector& SkillPoint)
{
	if (!Skill || Targets.Num() == 0 || !ownerCharacter) return false;
	if (!Skill->CanExecute()) return false;

	//AI가 조준한 실제 지점을 밀치기 기준점으로 기록, Caster 기준 스킬은 Execute가 시전자 위치를 직접 사용
	Skill->skillPointLocation = SkillPoint;

	//Ray/Cone은 조준 지점이 위치가 아니라 방향을 뜻하므로 시전자를 그 방향으로 돌림
	if (UAreaShapeLibrary::IsCasterAnchored(Skill))
	{
		FVector dir = SkillPoint - ownerCharacter->GetActorLocation();
		dir.Z = 0.f;
		if (!dir.IsNearlyZero())
		{
			ownerCharacter->SetActorRotation(dir.Rotation());
		}
	}

	//자원 차감은 1회만, 확정 시점 즉시
	Skill->BeginUse();

	//위협 프로파일에 기록
	if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		gm->RecordSkillUse(ownerCharacter, Skill);
	}

	//대상별 예측 계산·명중 판정·효과 적용은 커밋 시점에 실행
	StartPendingOnTargets(Skill, Targets);
	return true;
}

void USkillComponent::StartPendingOnTargets(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets)
{
	//이전 pending 대상이 섞이지 않도록 먼저 커밋한 뒤 등록
	CommitPendingExecution();

	//커밋 시점까지 대상 생존 불확실, 약참조 스냅샷
	pendingTargets.Reset();
	for (ACharacterBase* Target : Targets)
	{
		if (IsValid(Target)) pendingTargets.Add(Target);
	}

	//대상별 투사체가 먼저 적용한 대상은 이미 빠져 있다
	StartPendingExecution(Skill, Targets, [this, Skill]() { ApplyPendingTargets(Skill); });
}

void USkillComponent::ApplyToTarget(UActiveSkillBase* Skill, ACharacterBase* Target)
{
	if (!Skill || !IsValid(Target)) return;

	RecalculatePending(Skill, Target);
	Target->SetLastAttacker(ownerCharacter);
	//판정에서 사망하면 위치를 잃으므로 미리 기록
	const FVector hitLocation = Target->GetActorLocation();
	if (!Target->ReflectDamage()) return;
	Skill->SpawnHitEffect(hitLocation);
	if (IsValid(Target))
	{
		Skill->Execute(Target);
	}
}

void USkillComponent::ApplyPendingTargets(UActiveSkillBase* Skill)
{
	//적용 중 새 pending이 등록될 수 있어 먼저 비운다
	const TArray<TWeakObjectPtr<ACharacterBase>> targets = MoveTemp(pendingTargets);
	pendingTargets.Reset();
	for (const TWeakObjectPtr<ACharacterBase>& Weak : targets)
	{
		ApplyToTarget(Skill, Weak.Get());
	}
}

void USkillComponent::StartPendingExecution(UActiveSkillBase* Skill, const TArray<ACharacterBase*>& Targets, TFunction<void()>&& Action)
{
	//이전 pending이 남아있으면 먼저 커밋해 유실 방지
	CommitPendingExecution();

	pendingSkill = Skill;
	pendingAction = MoveTemp(Action);

	//인디케이터는 곧 파괴되므로 확정 시점 조준 지점을 커밋까지 보관, AI 직접 시전은 조준 없음
	bHasPendingAim = false;
	FVector aimPoint;
	if (GetAimPoint(aimPoint))
	{
		pendingAimPoint = aimPoint;
		bHasPendingAim = true;
	}

	//연출 카메라가 담을 지점, 시전자가 맨 앞이어야 프레이밍에서 빠지지 않는다
	//조준 지점은 점프 착지점·지면 사격점을 담기 위함, 조준이 없으면 기록된 시전 지점
	TArray<FVector> framePoints;
	if (ownerCharacter)
	{
		framePoints.Add(ownerCharacter->GetActorLocation());
		for (const ACharacterBase* target : Targets)
		{
			if (IsValid(target) && target != ownerCharacter) framePoints.Add(target->GetActorLocation());
		}
		if (bHasPendingAim) framePoints.Add(pendingAimPoint);
		else if (Skill) framePoints.Add(Skill->skillPointLocation);
	}

	//행동 시작 통지, 연출 카메라가 자리 잡을 시간만큼 시전을 늦춘다
	float startDelay = 0.f;
	if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		startDelay = gm->BeginAction(ownerCharacter, Skill, framePoints);
	}

	if (startDelay > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(presentationTimer, this, &USkillComponent::PlayPendingPresentation, startDelay, false);
		return;
	}
	PlayPendingPresentation();
}

void USkillComponent::PlayPendingPresentation()
{
	//대기 중 강제 커밋됐으면 연출 생략
	if (!pendingAction) return;

	UActiveSkillBase* Skill = pendingSkill;

	//시전자에 붙여 이동·도약을 따라가게 함
	if (Skill && ownerCharacter)
	{
		USceneComponent* root = ownerCharacter->GetRootComponent();
		if (Skill->castEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAttached(Skill->castEffect, root, NAME_None,
				FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset, true);
		}
		if (Skill->castSound)
		{
			UGameplayStatics::SpawnSoundAttached(Skill->castSound, root);
		}
	}

	float duration = 0.f;
	UAnimMontage* montage = Skill ? Skill->GetCommitMontage() : nullptr;
	if (montage && ownerCharacter)
	{
		pendingMontage = montage;
		duration = ownerCharacter->PlayAnimMontage(montage, Skill->montagePlayRate);
		if (duration > 0.f)
		{
			//노티파이 누락·중단 대비 몽타주 종료 시 커밋 폴백
			if (UAnimInstance* anim = ownerCharacter->GetMesh() ? ownerCharacter->GetMesh()->GetAnimInstance() : nullptr)
			{
				FOnMontageEnded endDelegate;
				endDelegate.BindUObject(this, &USkillComponent::OnSkillMontageEnded);
				anim->Montage_SetEndDelegate(endDelegate, montage);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] 몽타주 재생 실패: %s — ABP의 AnimGraph에 DefaultSlot 노드 필요"),
				*montage->GetName());
		}
	}

	//몽타주 없는 자가 버프는 상체 숙이기로 대신하고 정점에서 커밋
	if (duration <= 0.f && Skill && Skill->bBowOnCast && ownerCharacter)
	{
		UAnimInstanceBase* anim = ownerCharacter->GetMesh() ? Cast<UAnimInstanceBase>(ownerCharacter->GetMesh()->GetAnimInstance()) : nullptr;
		if (anim)
		{
			anim->PlayBow(Skill->bowDuration);
			GetWorld()->GetTimerManager().SetTimer(bowImpactTimer, this, &USkillComponent::OnSkillImpact, Skill->bowDuration * 0.5f, false);
			return;
		}
	}

	//몽타주가 없거나 재생 실패 시 즉시 커밋해 효과 증발 방지
	if (duration <= 0.f)
	{
		CommitPendingExecution();
	}
}

void USkillComponent::OnSkillImpact()
{
	//커밋 대기 중이 아니면 할 일 없음
	if (!pendingAction) return;

	//노티파이 연발 중이면 노티파이마다 다음 발
	if (bShotsByNotify && shotQueue.Num() > 0)
	{
		FireNextShot();
		return;
	}

	//이미 발사했으면 추가 임팩트 노티파이 무시
	if (activeProjectiles.Num() > 0 || shotQueue.Num() > 0) return;

	//투사체 멀티픽은 대상마다 한 발씩, 같은 대상을 거듭 골랐으면 그만큼 쏜다
	if (pendingSkill && pendingSkill->projectileClass && pendingSkill->selectMode == ESelectMode::SinglePick
		&& pendingSkill->pickCount > 1 && pendingTargets.Num() > 0)
	{
		shotQueue = pendingTargets;
		//임팩트 노티파이가 둘 이상이면 노티파이가 발사 시점, 아니면 간격 타이머
		bShotsByNotify = CountImpactNotifies(pendingMontage) > 1;
		FireNextShot();
		return;
	}

	PlayMuzzleEffects();

	//투사체가 있으면 발사만 하고 커밋은 도착 시점으로 미룸
	if (pendingSkill && pendingSkill->projectileClass && ownerCharacter)
	{
		//지면 지점은 반높이만큼 올리고, 대상 지점은 이미 캡슐 중심
		FVector to = pendingSkill->skillPointLocation;
		if (pendingSkill->selectMode != ESelectMode::SinglePick)
		{
			to.Z += ownerCharacter->GetSimpleCollisionHalfHeight();
		}
		if (LaunchProjectile(pendingSkill, to, TWeakObjectPtr<ACharacterBase>())) return;
	}

	CommitPendingExecution();
}

void USkillComponent::PlayMuzzleEffects()
{
	//발사 소켓이 있으면 총구에 붙이고, 없으면 시전자 루트
	if (pendingSkill && ownerCharacter)
	{
		USceneComponent* attachTo = ownerCharacter->GetRootComponent();
		FName socket = NAME_None;
		UStaticMeshComponent* weapon = ownerCharacter->GetWeaponMeshComponent();
		if (weapon && !pendingSkill->muzzleSocketName.IsNone() && weapon->DoesSocketExist(pendingSkill->muzzleSocketName))
		{
			attachTo = weapon;
			socket = pendingSkill->muzzleSocketName;
		}

		if (pendingSkill->muzzleEffect)
		{
			UNiagaraComponent* muzzleComp = UNiagaraFunctionLibrary::SpawnSystemAttached(pendingSkill->muzzleEffect, attachTo, socket,
				FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget, true);
			//소켓 축 대신 시전자 정면을 +X로, 위치만 총구를 따르고 방향은 고정(총구가 흔들려도 한쪽으로 쏠리지 않게)
			if (muzzleComp)
			{
				muzzleComp->SetUsingAbsoluteRotation(true);
				muzzleComp->SetWorldRotation(ownerCharacter->GetActorRotation());
				//목표까지 거리로 흩뿌림 길이를 맞춘다, 이 파라미터가 없는 시스템은 무시
				muzzleComp->SetVariableFloat(TEXT("User.TargetDistance"),
					FVector::Dist(muzzleComp->GetComponentLocation(), pendingSkill->skillPointLocation));
			}
		}
		if (pendingSkill->muzzleSound)
		{
			UGameplayStatics::SpawnSoundAttached(pendingSkill->muzzleSound, attachTo, socket,
				FVector::ZeroVector, EAttachLocation::SnapToTarget);
		}
	}
}

void USkillComponent::FireNextShot()
{
	if (!pendingAction || !pendingSkill || shotQueue.Num() == 0) return;

	const TWeakObjectPtr<ACharacterBase> target = shotQueue[0];
	shotQueue.RemoveAt(0);

	//남은 발은 노티파이 또는 간격 타이머가 쏘고, 마지막 발의 도착이 커밋한다
	//속도 0 투사체는 발사 즉시 도착해 큐를 보고 커밋하므로 예약을 먼저
	if (shotQueue.Num() > 0 && !bShotsByNotify)
	{
		GetWorld()->GetTimerManager().SetTimer(shotTimer, this, &USkillComponent::FireNextShot,
			FMath::Max(pendingSkill->multiProjectileInterval, 0.01f), false);
	}

	PlayMuzzleEffects();

	//쏠 수 없는 대상(사망 등)은 그 몫을 바로 처리
	if (!target.IsValid() || !LaunchProjectile(pendingSkill, target->GetActorLocation(), target))
	{
		OnProjectileArrived(nullptr, target);
	}
}

int32 USkillComponent::CountImpactNotifies(const UAnimMontage* Montage)
{
	if (!Montage) return 0;

	int32 count = 0;
	for (const FAnimNotifyEvent& event : Montage->Notifies)
	{
		if (event.Notify && event.Notify->IsA<UAnimNotify_SkillImpact>())
		{
			++count;
		}
	}
	return count;
}

bool USkillComponent::LaunchProjectile(UActiveSkillBase* Skill, const FVector& To, TWeakObjectPtr<ACharacterBase> Target)
{
	if (!Skill || !Skill->projectileClass || !ownerCharacter) return false;

	UWorld* world = GetWorld();
	if (!world) return false;

	const TArray<FVector> path = BuildProjectilePath(Skill, To);
	if (path.Num() < 2) return false;

	AProjectileBase* projectile = world->SpawnActor<AProjectileBase>(
		Skill->projectileClass, FTransform(path[0]));
	if (!projectile) return false;

	//속도가 0이면 Launch가 즉시 도착 통지하므로 등록·바인딩을 먼저
	activeProjectiles.Add(projectile);
	projectile->onArrived.BindUObject(this, &USkillComponent::OnProjectileArrived,
		TWeakObjectPtr<AProjectileBase>(projectile), Target);
	projectile->Launch(path, Skill->projectileSpeed);
	return true;
}

TArray<FVector> USkillComponent::BuildProjectilePath(const UActiveSkillBase* Skill, const FVector& To) const
{
	//발사 지점은 무기 메시의 소켓, 미지정·부재 시 캡슐 중심(미리보기 아치의 시작점과 같은 높이)
	FVector from = ownerCharacter->GetActorLocation();
	if (!Skill->muzzleSocketName.IsNone())
	{
		const UStaticMeshComponent* weapon = ownerCharacter->GetWeaponMeshComponent();
		if (weapon && weapon->DoesSocketExist(Skill->muzzleSocketName))
		{
			from = weapon->GetSocketLocation(Skill->muzzleSocketName);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("[SkillComponent] 무기 메시에 소켓 없음: %s — 캡슐 중심에서 발사"),
				*Skill->muzzleSocketName.ToString());
		}
	}

	//궤적 스킬(투척 등)은 미리보기와 같은 공식의 포물선으로
	if (Skill->arcApexRatio > 0.f)
	{
		const float horizDist = FVector2D(To.X - from.X, To.Y - from.Y).Size();
		return AAttackRangeIndicator::BuildArcPath(
			from, To, horizDist * Skill->arcApexRatio, projectileArcSamples);
	}

	//궤적이 없으면 일자 경로
	return { from, To };
}

void USkillComponent::OnProjectileArrived(TWeakObjectPtr<AProjectileBase> Projectile, TWeakObjectPtr<ACharacterBase> Target)
{
	activeProjectiles.Remove(Projectile.Get());

	//대상별 발사면 그 대상 몫만 지금 적용, 단발은 대상 없이 쏴서 커밋이 전부 적용
	//사망한 대상은 약참조가 끊겨도 명시적 null은 아니므로 대상별로 처리된다
	if (!Target.IsExplicitlyNull())
	{
		pendingTargets.RemoveSingle(Target);
		ApplyToTarget(pendingSkill, Target.Get());
	}

	if (activeProjectiles.Num() == 0 && shotQueue.Num() == 0)
	{
		CommitPendingExecution();
	}
}

void USkillComponent::DiscardActiveProjectiles()
{
	//효과는 커밋이 적용하므로 도착 통지를 끊고 제거
	for (const TObjectPtr<AProjectileBase>& projectile : activeProjectiles)
	{
		if (!IsValid(projectile)) continue;
		projectile->onArrived.Unbind();
		projectile->Destroy();
	}
	activeProjectiles.Reset();
}

void USkillComponent::CommitPendingExecution()
{
	if (!pendingAction) return;

	//카메라 도착 전 강제 커밋이면 늦춰 둔 연출을 취소
	GetWorld()->GetTimerManager().ClearTimer(presentationTimer);
	GetWorld()->GetTimerManager().ClearTimer(bowImpactTimer);

	//커밋 액션이 새 pending을 등록할 수 있어 실행 전에 해제
	TFunction<void()> action = MoveTemp(pendingAction);
	pendingAction = nullptr;

	UActiveSkillBase* skill = pendingSkill;
	pendingSkill = nullptr;
	pendingMontage = nullptr;

	//턴 종료 등 외부 강제 커밋으로 투사체 도착을 앞질렀다면 남은 발 취소·비행 중 투사체 폐기
	//그 대상들은 pendingTargets에 남아 있어 action이 적용한다
	GetWorld()->GetTimerManager().ClearTimer(shotTimer);
	shotQueue.Reset();
	bShotsByNotify = false;
	DiscardActiveProjectiles();

	//스킬 자체 효과(포복 토글 등) 먼저, 이후 대상별 효과
	if (skill)
	{
		skill->OnCommit();
		//OnCommit 오버라이드가 Super를 생략해도 출력되도록 여기서 처리
		for (const TObjectPtr<UNiagaraSystem>& effect : skill->pointEffects)
		{
			if (effect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), effect, skill->skillPointLocation);
			}
		}
		if (skill->pointSound)
		{
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), skill->pointSound, skill->skillPointLocation);
		}
	}
	action();

	//효과 적용 완료 통지, 결과 확정은 카메라 복귀·착지 후 비동기로
	if (ABattleGameMode* gm = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		gm->NotifyActionCommitted();
	}
}

bool USkillComponent::GetAimPoint(FVector& OutPoint) const
{
	if (!ownerCharacter) return false;

	//조준 중, 본인 대상 스킬은 회전 없음
	if (currentSkill && currentSkill->selectMode != ESelectMode::Self && attackRangeIndicator)
	{
		//Ray/Cone은 인디케이터가 시전자 위치에 겹치므로 회전 방향 앞쪽 지점을 쓴다
		if (UAreaShapeLibrary::IsCasterAnchored(currentSkill))
		{
			OutPoint = ownerCharacter->GetActorLocation() + attackRangeIndicator->GetActorForwardVector() * 1000.f;
			return true;
		}

		OutPoint = attackRangeIndicator->GetActorLocation();
		//지면 지점은 캐릭터 중심 높이로 올려 같은 높이면 피치 0
		if (!attackRangeIndicator->GetSnappedTarget())
		{
			OutPoint.Z += ownerCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}
		return true;
	}

	//커밋 대기 중, 확정 시점 지점 유지
	if (bHasPendingAim && HasPendingExecution())
	{
		OutPoint = pendingAimPoint;
		return true;
	}
	return false;
}

void USkillComponent::OnSkillMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//현재 pending 소속이 아닌 몽타주의 종료는 무시 (이전 스킬 몽타주 블렌드아웃 등)
	if (Montage != pendingMontage) return;

	//같은 몽타주 재시작으로 인한 이전 인스턴스 중단이면 새 인스턴스의 종료를 기다림
	if (bInterrupted && ownerCharacter && ownerCharacter->GetCurrentMontage() == Montage) return;

	//노티파이가 모자라 남은 발은 간격 타이머로 이어 쏜다
	if (bShotsByNotify && shotQueue.Num() > 0)
	{
		bShotsByNotify = false;
		FireNextShot();
		return;
	}

	//투사체 비행·연발 대기 중이면 마지막 도착 커밋을 기다림, 몽타주가 먼저 끝나도 효과는 착탄에 맞춤
	if (activeProjectiles.Num() > 0 || shotQueue.Num() > 0) return;

	//노티파이가 이미 커밋했으면 no-op
	CommitPendingExecution();
}

void USkillComponent::SpawnIndicators(UActiveSkillBase* Skill)
{
	if (!ownerCharacter) return;

	UWorld* World = GetWorld();
	if (!World) return;

	const FVector CasterLocation = ownerCharacter->GetActorLocation();

	//시전 가능 범위 인디케이터 생성, 점프는 현재 이동력으로 갈 수 있는 범위로 사거리 원 표시
	//표시 여부도 실효 사거리로 판단, pickRange는 점프에서 상한이 아님
	//Ray/Cone은 형태 자체가 사거리라 별도 사거리 원을 띄우면 오히려 혼란
	const float effectivePickRange = Skill->GetEffectivePickRange();
	if (skillRangeIndicatorClass && effectivePickRange > 0.f && Skill->selectMode != ESelectMode::Self
		&& !UAreaShapeLibrary::IsCasterAnchored(Skill))
	{
		skillRangeIndicator = World->SpawnActor<ASkillRangeIndicator>(
			skillRangeIndicatorClass, FTransform(CasterLocation));
		if (skillRangeIndicator)
		{
			skillRangeIndicator->InitRange(effectivePickRange);
		}
	}

	//공격 범위 인디케이터 생성
	if (attackRangeIndicatorClass)
	{
		attackRangeIndicator = World->SpawnActor<AAttackRangeIndicator>(
			attackRangeIndicatorClass, FTransform(CasterLocation));
		if (attackRangeIndicator)
		{
			attackRangeIndicator->InitIndicator(ownerCharacter, Skill);
		}
	}
}

void USkillComponent::DestroyIndicators()
{
	if (IsValid(skillRangeIndicator))
	{
		skillRangeIndicator->Destroy();
		skillRangeIndicator = nullptr;
	}
	if (IsValid(attackRangeIndicator))
	{
		attackRangeIndicator->Destroy();
		attackRangeIndicator = nullptr;
	}
}
