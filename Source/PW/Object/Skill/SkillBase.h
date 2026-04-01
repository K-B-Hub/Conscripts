// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillBase.generated.h"

class ACharacterBase;

// 모든 스킬(액티브, 패시브)의 공통 기반 데이터
UCLASS(Abstract, BlueprintType)
class PW_API USkillBase : public UObject
{
	GENERATED_BODY()

public:
	// 스킬 이름 (UI 표시용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	FText skillName;

	// 스킬 설명 (UI 툴팁용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	FText skillDescription;

	// 스킬 아이콘 (UI 버튼에 표시)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	TObjectPtr<UTexture2D> skillIcon;

	// 스킬 소유 캐릭터 (GC 등록을 위해 UPROPERTY 필요)
	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> owner;

	// 소유자 설정 — SkillComponent에서 스킬 인스턴스 생성 후 호출
	void SetOwner(ACharacterBase* InOwner);

	ACharacterBase* GetOwner() const;
};
