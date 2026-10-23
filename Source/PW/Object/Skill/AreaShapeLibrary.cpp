//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Skill/AreaShapeLibrary.h"
#include "Object/Skill/ActiveSkillBase.h"

bool UAreaShapeLibrary::IsCasterAnchored(const UActiveSkillBase* Skill)
{
	if (!Skill || Skill->areaTarget == EAreaTarget::None) return false;

	//Ray/Cone은 시전자에 붙어 방향만 돌리므로 조준 지점이 사거리가 아니라 방향을 뜻함
	return Skill->areaForm == EAreaForm::Ray || Skill->areaForm == EAreaForm::Cone;
}

float UAreaShapeLibrary::GetOuterRadius(const UActiveSkillBase* Skill)
{
	if (!Skill || Skill->areaTarget == EAreaTarget::None) return 0.f;

	switch (Skill->areaForm)
	{
	case EAreaForm::Circle:
		return Skill->areaParameter1;
	case EAreaForm::Cone:
		return Skill->areaParameter1;
	case EAreaForm::Ray:
		//원점이 시전자이므로 가장 먼 지점은 끝단 모서리
		return FMath::Sqrt(Skill->areaParameter2 * Skill->areaParameter2
			+ Skill->areaParameter1 * Skill->areaParameter1 * 0.25f);
	}
	return 0.f;
}

bool UAreaShapeLibrary::IsInAreaShape(const UActiveSkillBase* Skill, const FVector& Origin,
	const FVector& Forward, const FVector& TargetLocation)
{
	if (!Skill || Skill->areaTarget == EAreaTarget::None) return false;

	FVector toTarget = TargetLocation - Origin;
	toTarget.Z = 0.f;

	if (Skill->areaForm == EAreaForm::Circle)
	{
		return toTarget.SizeSquared() <= Skill->areaParameter1 * Skill->areaParameter1;
	}

	//Ray/Cone은 전방이 없으면 형태가 정의되지 않음
	FVector forward2D = Forward;
	forward2D.Z = 0.f;
	if (forward2D.IsNearlyZero()) return false;
	forward2D.Normalize();

	if (Skill->areaForm == EAreaForm::Ray)
	{
		//축 투영이 0~길이, 축에서의 수직 거리가 폭의 절반 이내
		const float along = FVector::DotProduct(toTarget, forward2D);
		if (along < 0.f || along > Skill->areaParameter2) return false;
		const float perp = FMath::Abs(FVector::CrossProduct(forward2D, toTarget).Z);
		return perp <= Skill->areaParameter1 * 0.5f;
	}

	//Cone, 반지름 안이면서 전방과의 각도가 중심각 절반 이내
	if (toTarget.SizeSquared() > Skill->areaParameter1 * Skill->areaParameter1) return false;
	FVector dirToTarget = toTarget;
	//원점과 겹친 대상은 각도를 정의할 수 없어 범위 안으로 간주
	if (dirToTarget.IsNearlyZero()) return true;
	dirToTarget.Normalize();
	const float cosHalf = FMath::Cos(FMath::DegreesToRadians(Skill->areaParameter2 * 0.5f));
	return FVector::DotProduct(forward2D, dirToTarget) >= cosHalf;
}