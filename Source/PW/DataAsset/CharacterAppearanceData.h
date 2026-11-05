//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterAppearanceData.generated.h"

class USkeletalMesh;
class UStaticMesh;
class UMaterialInterface;

//리더 포즈를 따라가는 스켈레탈 파츠 하나
USTRUCT(BlueprintType)
struct FAppearancePart
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USkeletalMesh> Mesh;

	//머티리얼 슬롯 순서, 빈 칸은 메시 기본 머티리얼
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UMaterialInterface>> Materials;
};

//본에 붙는 스태틱 부착물(헤드셋·고글·라이트 등)
USTRUCT(BlueprintType)
struct FAppearanceAttachment
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TObjectPtr<UMaterialInterface>> Materials;

	//리더 메시의 본 또는 소켓
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Socket;

	//Socket 기준 상대 트랜스폼
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform Offset;
};

//캐릭터 외형 프리셋, 커스터마이징 결과를 구워 넣는다
//아군은 캐릭터 BP마다, 적은 직업 BP마다 하나씩 지정
UCLASS(BlueprintType)
class PW_API UCharacterAppearanceData : public UDataAsset
{
	GENERATED_BODY()

public:
	//GetMesh()에 들어갈 메시, 애니메이션이 여기서 돈다. 메시를 비우면 BP 메시 유지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	FAppearancePart leader;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TArray<FAppearancePart> parts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Appearance")
	TArray<FAppearanceAttachment> attachments;
};
