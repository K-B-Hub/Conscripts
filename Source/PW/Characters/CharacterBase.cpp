// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "ActorComponent/AilmentComponent.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Widget/HealthWidget.h"
#include "Widget/SkillInfoWidget.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"

//데미지 산식의 단일 출처, CalculateDamage와 PreviewDamage가 공유
static void ComputeDamageNumbers(const ACharacterBase* Target,
	float Damage, float Accuracy, float Critical, int32 DamageAmplfication, int32 Penetration,
	ESkillType SkillType, EPickTeam PickTeam,
	float& OutDmg, float& OutAccuracy, float& OutCritical)
{
	if (!Target)
	{
		OutDmg = 0.f; OutAccuracy = 0.f; OutCritical = 0.f;
		return;
	}

	if (SkillType == ESkillType::Buff)
	{
		OutDmg = 0.f;
		//아군 전용 버프는 회피 차감 없이 스킬 명중 그대로
		OutAccuracy = (PickTeam == EPickTeam::AllyOnly) ? Accuracy : (Accuracy - Target->GetEvasion());
		OutAccuracy = FMath::Clamp(OutAccuracy, 0.f, 100.f);
		OutCritical = 0.f;
		return;
	}
	if (SkillType == ESkillType::Ailment)
	{
		OutDmg = 0.f;
		OutAccuracy = FMath::Clamp(Accuracy - Target->GetEvasion(), 0.f, 100.f);
		OutCritical = 0.f;
		return;
	}

	OutDmg = (Damage * (1 + DamageAmplfication / 100.f) - Target->GetDef() * (1 - Penetration / 100.f))
	         * (1 - Target->GetDamageReduction() / 100.f);
	if (OutDmg <= 0.f) OutDmg = 0.f;
	OutAccuracy = FMath::Clamp(Accuracy - Target->GetEvasion(), 0.f, 100.f);
	OutCritical = Critical;
}

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArmComponent->TargetArmLength = cameraArmLength;
	springArmComponent->SetRelativeRotation(FRotator(cameraPitchAngle, 0.f, 0.f));
	springArmComponent->bUsePawnControlRotation = false;
	springArmComponent->bDoCollisionTest = false;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComponent->SetupAttachment(springArmComponent, USpringArmComponent::SocketName);
	cameraComponent->bUsePawnControlRotation = false;

	healthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	healthWidgetComponent->SetupAttachment(RootComponent);
	healthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); //캡슐 상단 위
	healthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	healthWidgetComponent->SetDrawSize(FVector2D(150.f, 20.f));

	skillInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SkillInfoWidget"));
	skillInfoWidgetComponent->SetupAttachment(RootComponent);
	skillInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 90.f)); //캐릭터 중앙 부근을 앵커로
	skillInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//TODO: Pivot을 조정해 앵커 기준 오른편 아래로 위젯이 펼쳐지게 한다 (기본 0.5,0.5 = 중앙)
	skillInfoWidgetComponent->SetPivot(FVector2D(1, -1));
	skillInfoWidgetComponent->SetDrawSize(FVector2D(200.f, 60.f));
	skillInfoWidgetComponent->SetVisibility(false);

	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket_R"));
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	skillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	if (skillComponent)
	{
		skillComponent->SetOwner(this);
	}

	buffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	ailmentComponent = CreateDefaultSubobject<UAilmentComponent>(TEXT("AilmentComponent"));
	passiveSkillComponent = CreateDefaultSubobject<UPassiveSkillComponent>(TEXT("PassiveSkillComponent"));

	GetCapsuleComponent()->SetCanEverAffectNavigation(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bUseControllerRotationYaw = false; 
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->InitHealth(maxHp, hp);
		healthWidget = HealthWidget;
	}
	SetDefaultStats();
	SetDefaultSkills();
	//패시브는 SetDefaultStats 이후에 등록, 파생 스탯 덮어쓰기 방지
	SetDefaultPassives();
}

void ACharacterBase::SetDefaultSkills()
{
	//파생 클래스에서 구현
}

void ACharacterBase::SetDefaultPassives()
{
	//파생 클래스에서 구현
}

void ACharacterBase::SetDefaultStats()
{
	accuracy = skill * 1.2;
	evasion = speed * 1.2;
	critical = skill * 0.5;
	if (skillComponent)
	{
		skillComponent->CalcSkillStats();
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ACharacterBase::GetTurnOrder() const
{
	return speed + FMath::RandRange(0, speed);
}

void ACharacterBase::GetEXP(bool bIsKill)
{
	if (bIsKill)
	{
		exp += 55;			//추후 경험치 공식 적용 필요
	}
	else
	{
		exp += 20;
	}
	if (exp >= maxExp)
	{
		while (exp >= maxExp)
		{
			LevelUp();
			exp -= 100;
		}
	}
}

void ACharacterBase::LevelUp()
{
	level++;

	if (FMath::RandRange(1, 100) <= hpGrowth)
	{
		maxHp++; hp++;
		if (IsValid(healthWidget))
			healthWidget->InitHealth(maxHp, hp);
	}
	if (FMath::RandRange(1, 100) <= atkGrowth)       { atk++; }
	if (FMath::RandRange(1, 100) <= speedGrowth)     { speed++; }
	if (FMath::RandRange(1, 100) <= skillGrowth)     { skill++; }
	if (FMath::RandRange(1, 100) <= defGrowth)       { def++; }
	if (FMath::RandRange(1, 100) <= mentalityGrowth) { mentality++; }

	//파생 스탯 재계산
	SetDefaultStats();

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 레벨업 → Lv.%d"), *GetName(), level);
}

void ACharacterBase::ReduceActionPoint(int32 amount)
{
	currentActionPoint = FMath::Clamp(currentActionPoint - amount, 0, actionPoint);
}

void ACharacterBase::ReduceBattleResource(int32 amount)
{
	battleResource = FMath::Clamp(battleResource - amount, 0, battleResource);
}

void ACharacterBase::ConsumeMovingPoint(float meters)
{
	currentMovingPoint = FMath::Max(0.f, currentMovingPoint - meters);
}

void ACharacterBase::CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication,
                                     int Penetration, ESkillType SkillType, EPickTeam PickTeam)
{
	//공용 헬퍼 호출, 결과를 pending* 멤버에 기입 (인디케이터 위젯·ReflectDamage 경로)
	pendingSkillType = SkillType;
	ComputeDamageNumbers(this, Damage, Accuracy, Critical, DamageAmplfication, Penetration,
		SkillType, PickTeam, pendingDMG, pendingAccuracy, pendingCritical);
}

FDamageResult ACharacterBase::PreviewDamage(const UActiveSkillBase* Skill,
                                            const ACharacterBase* Attacker,
                                            const FVector& AttackerLocation) const
{
	FDamageResult result;
	if (!Skill || !Attacker) return result;

	result.SkillType = Skill->skillType;

	//스킬 calc 값 로컬 카피, 외부 상태 미변경 보장
	float dmg  = Skill->calcDamage;
	int32 amp  = Skill->calcDamageAmplfication;
	int32 pen  = Skill->calcPenetration;
	float acc  = Skill->calcAccuracy;
	float crit = Skill->calcCritical;

	//캐스터 측 BeforeDamageCalc 패시브 보너스 합산
	if (UPassiveSkillComponent* PSC = Attacker->GetPassiveSkillComponent())
	{
		PSC->DispatchBeforeDamageCalc(const_cast<ACharacterBase*>(this),
			Skill->skillType, Skill->damageType, dmg, amp, pen, acc, crit);
	}

	float outDmg = 0.f, outAcc = 0.f, outCrit = 0.f;
	ComputeDamageNumbers(this, dmg, acc, crit, amp, pen, Skill->skillType, Skill->pickTeam,
		outDmg, outAcc, outCrit);

	result.HitChance   = outAcc;
	result.CritChance  = outCrit;
	result.NormalDamage = FMath::RoundToInt(outDmg);
	result.CritDamage   = FMath::RoundToInt(outDmg * 2.f);   //ReflectDamage 산식과 일치
	//두 단계 처치 가능성, AI는 확정 처치와 운빨 처치를 다르게 가중
	result.bCanKill     = (result.NormalDamage >= hp);
	result.bCanCritKill = !result.bCanKill && (result.CritDamage >= hp);

	return result;
}

bool ACharacterBase::ReflectDamage()
{
	if (pendingSkillType == ESkillType::Buff || pendingSkillType == ESkillType::Ailment)
	{
		float hitRoll = FMath::FRandRange(0.f, 100.f);
		if (hitRoll > pendingAccuracy)
		{
			UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 버프 or 상태이상 회피"), *GetName());
			return false;
		}
		return true;
	}

	float hitRoll = FMath::FRandRange(0.f, 100.f);
	if (hitRoll > pendingAccuracy)
	{
		UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 공격 회피"), *GetName());
		return false;
	}
	float critRoll = FMath::FRandRange(0.f, 100.f);
	int32 finalDamage = (critRoll <= pendingCritical)
		? FMath::RoundToInt(pendingDMG * 2.f)
		: FMath::RoundToInt(pendingDMG);

	ReceiveDamage(finalDamage, true);

	return true;
}

void ACharacterBase::SetLastAttacker(ACharacterBase* Attacker)
{
	if (IsValid(Attacker))
	{
		lastAttacker = Attacker;
	}
}

void ACharacterBase::OnMoveStateChanged(bool newIsMoved)
{
	if (isMoved == newIsMoved) return;

	//이전 상태 보너스 revert, isMoved 갱신, 새 상태 보너스 apply 순서로 처리
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchBeforeMove(isMoved, -1);
	}
	isMoved = newIsMoved;
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchBeforeMove(isMoved, +1);
	}
	if (skillComponent)
	{
		skillComponent->CalcSkillStats();
	}
}

void ACharacterBase::InitTurn()
{
	currentActionPoint = actionPoint;
	currentMovingPoint = movingPoint;

	//이전 턴 이동 상태를 false로 전이
	OnMoveStateChanged(false);

	if (buffComponent)
	{
		buffComponent->OnTurnStart();
	}
	if (ailmentComponent)
	{
		ailmentComponent->OnTurnStart();
	}

	//턴 시작 Conditional 패시브, AP/이동력 리셋 이후 확정 상태 기준으로 평가
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchConditional(EConditionalType::TurnStart);
	}
}

void ACharacterBase::EndTurn()
{
	//턴 종료 Conditional 패시브, 버프/상태이상 차감 전 상태에서 평가
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchConditional(EConditionalType::TurnEnd);
	}

	if (buffComponent)
	{
		buffComponent->OnTurnEnd();
	}
	if (ailmentComponent)
	{
		ailmentComponent->OnTurnEnd();
	}
}

void ACharacterBase::ApplyBuffDelta(const UBuffBase* buff, int32 sign)
{
	if (!buff || (sign != 1 && sign != -1)) return;

	//최대 체력 변경, 현재 체력은 새 max에 클램프
	maxHp += buff->hp * sign;
	if (maxHp <= 0) maxHp = 1;
	hp = FMath::Clamp(hp, 1, maxHp);
	if (healthWidget)
	{
		healthWidget->InitHealth(maxHp, hp);
	}

	//기본 능력치
	atk += buff->atk * sign;
	speed += buff->speed * sign;
	skill += buff->skill * sign;
	def += buff->def * sign;
	movingPoint += buff->movingPoint * sign;
	currentMovingPoint += buff->movingPoint * sign;
	currentMovingPoint = FMath::Clamp(currentMovingPoint, 0, movingPoint);
	mentality += buff->mentality * sign;

	//행동력 최대치 변경, 현재치는 새 max에 클램프
	actionPoint += buff->actionPoint * sign;
	currentActionPoint += buff->actionPoint * sign;
	currentActionPoint = FMath::Clamp(currentActionPoint, 0, actionPoint);

	//피해 보정
	damageReduction += buff->damageReduction * sign;
	damageAmplification += buff->damageAmplification * sign;
	penetration += buff->penetration * sign;
	sight += buff->sight * sign;

	//전투 파생 스탯은 직접 가감, 자동 재계산은 하지 않음
	accuracy += buff->accuracy * sign;
	evasion += buff->evasion * sign;
	critical += buff->critical * sign;

	//스킬 측 계산값 재계산 알림, 사망 상태에서는 owner가 stale weak ptr이라 스킵
	if (skillComponent && !IsDead())
	{
		skillComponent->CalcSkillStats();
	}
}

void ACharacterBase::ApplyPassiveStatDelta(const UPassiveSkillBase* passive, int32 sign)
{
	if (!passive || (sign != 1 && sign != -1)) return;

	UE_LOG(LogTemp, Log, TEXT("[Passive] %s 패시브 '%s' 적용 시작 (sign=%d)"),
		*GetName(),
		*passive->GetClass()->GetName(),
		sign);

	//적용 전 스냅샷
	const int32 beforeMaxHp = maxHp;
	const int32 beforeHp = hp;
	const int32 beforeAtk = atk;
	const int32 beforeSpeed = speed;
	const int32 beforeSkill = skill;
	const int32 beforeDef = def;
	const float beforeMovingPoint = movingPoint;
	const int32 beforeMentality = mentality;
	const int32 beforeActionPoint = actionPoint;
	const int32 beforeDamageReduction = damageReduction;
	const int32 beforeDamageAmplification = damageAmplification;
	const int32 beforePenetration = penetration;
	const float beforeSight = sight;
	const float beforeAccuracy = accuracy;
	const float beforeEvasion = evasion;
	const float beforeCritical = critical;

	//최대 HP 증가량만큼 현재 HP도 함께 증가, 패시브는 영구이므로 등록 시점이 곧 성장
	const int32 hpDelta = passive->hp * sign;
	maxHp += hpDelta;
	if (maxHp <= 0) maxHp = 1;
	hp += hpDelta;
	hp = FMath::Clamp(hp, 1, maxHp);
	if (healthWidget)
	{
		healthWidget->InitHealth(maxHp, hp);
	}

	//기본 능력치
	atk += passive->atk * sign;
	speed += passive->speed * sign;
	skill += passive->skill * sign;
	def += passive->def * sign;
	movingPoint += passive->movingPoint * sign;
	currentMovingPoint += passive->movingPoint * sign;
	currentMovingPoint = FMath::Clamp(currentMovingPoint, 0, movingPoint);
	mentality += passive->mentality * sign;

	//행동력 최대치 변경, 현재치도 같이 갱신 후 클램프
	actionPoint += passive->actionPoint * sign;
	currentActionPoint += passive->actionPoint * sign;
	currentActionPoint = FMath::Clamp(currentActionPoint, 0, actionPoint);

	//피해 보정
	damageReduction += passive->damageReduction * sign;
	damageAmplification += passive->damageAmplification * sign;
	penetration += passive->penetration * sign;
	sight += passive->sight * sign;

	//전투 파생 스탯은 직접 가감, SetDefaultStats 재호출하지 않음
	accuracy += passive->accuracy * sign;
	evasion += passive->evasion * sign;
	critical += passive->critical * sign;

	if (skillComponent && !IsDead())
	{
		skillComponent->CalcSkillStats();
	}

	//변경된 스탯만 로그 출력
	if (beforeMaxHp != maxHp || beforeHp != hp)
		UE_LOG(LogTemp, Log, TEXT("  HP: %d/%d -> %d/%d"), beforeHp, beforeMaxHp, hp, maxHp);
	if (beforeAtk != atk)
		UE_LOG(LogTemp, Log, TEXT("  atk: %d -> %d"), beforeAtk, atk);
	if (beforeSpeed != speed)
		UE_LOG(LogTemp, Log, TEXT("  speed: %d -> %d"), beforeSpeed, speed);
	if (beforeSkill != skill)
		UE_LOG(LogTemp, Log, TEXT("  skill: %d -> %d"), beforeSkill, skill);
	if (beforeDef != def)
		UE_LOG(LogTemp, Log, TEXT("  def: %d -> %d"), beforeDef, def);
	if (!FMath::IsNearlyEqual(beforeMovingPoint, movingPoint))
		UE_LOG(LogTemp, Log, TEXT("  movingPoint: %.2f -> %.2f"), beforeMovingPoint, movingPoint);
	if (beforeMentality != mentality)
		UE_LOG(LogTemp, Log, TEXT("  mentality: %d -> %d"), beforeMentality, mentality);
	if (beforeActionPoint != actionPoint)
		UE_LOG(LogTemp, Log, TEXT("  actionPoint: %d -> %d"), beforeActionPoint, actionPoint);
	if (beforeDamageReduction != damageReduction)
		UE_LOG(LogTemp, Log, TEXT("  damageReduction: %d -> %d"), beforeDamageReduction, damageReduction);
	if (beforeDamageAmplification != damageAmplification)
		UE_LOG(LogTemp, Log, TEXT("  damageAmplification: %d -> %d"), beforeDamageAmplification, damageAmplification);
	if (beforePenetration != penetration)
		UE_LOG(LogTemp, Log, TEXT("  penetration: %d -> %d"), beforePenetration, penetration);
	if (!FMath::IsNearlyEqual(beforeSight, sight))
		UE_LOG(LogTemp, Log, TEXT("  sight: %.2f -> %.2f"), beforeSight, sight);
	if (!FMath::IsNearlyEqual(beforeAccuracy, accuracy))
		UE_LOG(LogTemp, Log, TEXT("  accuracy: %.2f -> %.2f"), beforeAccuracy, accuracy);
	if (!FMath::IsNearlyEqual(beforeEvasion, evasion))
		UE_LOG(LogTemp, Log, TEXT("  evasion: %.2f -> %.2f"), beforeEvasion, evasion);
	if (!FMath::IsNearlyEqual(beforeCritical, critical))
		UE_LOG(LogTemp, Log, TEXT("  critical: %.2f -> %.2f"), beforeCritical, critical);
}

void ACharacterBase::ReceiveDamage(int32 Amount, bool bIsLethal)
{
	if (IsDead()) return;

	const int32 hpBefore = hp;
	const int32 minHp = bIsLethal ? 0 : 1;
	hp = FMath::Clamp(hp - Amount, minHp, maxHp);

	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->ApplyDamage(Amount);
	}

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 데미지 %d 수신 → 잔여 체력 %d / %d"), *GetName(), Amount, hp, maxHp);

	//Damaged Conditional, hp 변화가 있었을 때 발동(회복 포함), 사망 처리 전
	if (hp != hpBefore && passiveSkillComponent)
	{
		passiveSkillComponent->DispatchConditional(EConditionalType::Damaged);
	}

	//공격자 측 AfterDamage 패시브, 사망 처리 전에 발동시켜 AfterSlay와 순서 보장
	//스킬 적중에 의한 치명 피해일 때만 발동, 회복이나 비치명 데미지는 제외
	if (bIsLethal && Amount > 0)
	{
		if (ACharacterBase* attacker = lastAttacker.Get())
		{
			if (UPassiveSkillComponent* attackerPSC = attacker->GetPassiveSkillComponent())
			{
				attackerPSC->DispatchAfterDamage(this);
			}
		}
	}

	if (hp <= 0)
	{
		HandleDeath();
	}
}

void ACharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 사망"), *GetName());

	//공격자 측 AfterSlay 패시브 디스패치, 처치 지점 위치 전달
	if (ACharacterBase* attacker = lastAttacker.Get())
	{
		if (UPassiveSkillComponent* passivecomponent = attacker->GetPassiveSkillComponent())
		{
			passivecomponent->DispatchAfterSlay(GetActorLocation());
		}
	}

	OnCharacterDeath.Broadcast(this);

	//NavMesh 장애물 해제
	SetNavObstacleEnabled(false);

	//충돌 비활성화, Destroy 전 오버랩 정리 유도
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Destroy();
}

void ACharacterBase::ShowSkillInfo()
{
	skillInfoWidgetComponent->SetVisibility(true);
	if (USkillInfoWidget* InfoWidget = Cast<USkillInfoWidget>(skillInfoWidgetComponent->GetWidget()))
	{
		InfoWidget->UpdateInfo(pendingDMG, pendingAccuracy, pendingCritical, pendingSkillType);
	}
}

void ACharacterBase::HideSkillInfo()
{
	skillInfoWidgetComponent->SetVisibility(false);
}

void ACharacterBase::ClearPendingDamage()
{
	pendingDMG = 0.f;
	pendingAccuracy = 0.f;
	pendingCritical = 0.f;
	pendingSkillType = ESkillType::Melee;
}

void ACharacterBase::SetNavObstacleEnabled(bool bEnabled)
{
	GetCapsuleComponent()->SetCanEverAffectNavigation(bEnabled);

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		NavSys->UpdateActorInNavOctree(*this);
	}
}

UStaticMeshComponent* ACharacterBase::GetWeaponMeshComponent() const
{
	return WeaponMeshComp;
}
