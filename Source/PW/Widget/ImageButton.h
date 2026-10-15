// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ImageButton.generated.h"

class UImage;
class UTexture2D;

//버튼의 마우스 상태, 비활성은 상태가 아니라 틴트로 표현한다
UENUM()
enum class EImageButtonState : uint8
{
	Normal,
	Hovered,
	Pressed
};

//상태가 바뀔 때 알린다, 버튼 위에 얹은 아이콘을 같이 전환하는 쪽이 구독한다
DECLARE_DELEGATE_OneParam(FOnImageButtonStateChanged, EImageButtonState);

//버튼 한 상태 묶음, 런타임에 세트를 갈아끼우는 쪽(진영·턴 상태별 아트 등)에서 쓴다
USTRUCT(BlueprintType)
struct FImageButtonArt
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "ImageButton")
	TObjectPtr<UTexture2D> normal = nullptr;

	UPROPERTY(EditAnywhere, Category = "ImageButton")
	TObjectPtr<UTexture2D> hovered = nullptr;

	UPROPERTY(EditAnywhere, Category = "ImageButton")
	TObjectPtr<UTexture2D> pressed = nullptr;
};

//프로젝트 공용 버튼, 일반 UButton을 그대로 대체한다
//
//1) 상태별 아트를 자식 UImage로 그린다 (텍스처 미지정이면 기존 버튼 스타일 그대로 동작)
//   배경 브러시는 geometry를 채우므로 상태마다 캔버스 크기가 다른 아트를 담을 수 없다.
//   아트를 콘텐츠 이미지로 내리면 각 상태가 자기 네이티브 크기 × 배율로 그려져
//   중앙을 기준으로 바깥으로 자란다. 바깥을 SizeBox로 고정하면 주변 레이아웃은 움직이지 않는다.
//2) 호버 시 라벨을 지정 픽셀만큼 내린다 (FButtonStyle에는 Hovered 패딩이 없어 코드로 처리)
UCLASS()
class PW_API UImageButton : public UButton
{
	GENERATED_BODY()

public:
	//상태 아트 세트를 런타임에 교체, 현재 보이는 상태도 즉시 갱신된다
	void SetStateTextures(const FImageButtonArt& InArt);

	//프레임 위에 얹은 이미지를 같은 상태로 맞추고 싶을 때 구독한다
	FOnImageButtonStateChanged OnStateChanged;

	EImageButtonState GetButtonState() const { return currentState; }

	virtual void SynchronizeProperties() override;
	virtual void SetIsEnabled(bool bInIsEnabled) override;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;

	//상태별 아트, 셋 다 비우면 아트 기능을 쓰지 않고 버튼 스타일 브러시가 그대로 보인다
	UPROPERTY(EditAnywhere, Category = "ImageButton|Art")
	TObjectPtr<UTexture2D> normalTexture;

	UPROPERTY(EditAnywhere, Category = "ImageButton|Art")
	TObjectPtr<UTexture2D> hoveredTexture;

	UPROPERTY(EditAnywhere, Category = "ImageButton|Art")
	TObjectPtr<UTexture2D> pressedTexture;

	//아트 네이티브 해상도에 곱할 배율, 상태 간 크기 비율이 보존된다
	UPROPERTY(EditAnywhere, Category = "ImageButton|Art", meta = (ClampMin = "0.01"))
	float textureScale = 1.f;

	//비활성 시 아트에 입히는 틴트, 아트를 쓸 때만 적용된다
	UPROPERTY(EditAnywhere, Category = "ImageButton|Art")
	FLinearColor disabledTint = FLinearColor(0.4f, 0.4f, 0.4f, 1.f);

	//호버 시 라벨을 내릴 픽셀, 0이면 내리지 않는다
	UPROPERTY(EditAnywhere, Category = "ImageButton|Hover")
	float hoverTextOffsetY = 1.f;

	//내릴 대상 위젯 이름(옵션). 비우면 버튼의 콘텐츠를 쓴다
	//버튼과 라벨이 Overlay 형제인 배치에서 지정한다
	UPROPERTY(EditAnywhere, Category = "ImageButton|Hover")
	FName hoverTextName;

private:
	UFUNCTION()
	void HandleHovered();

	UFUNCTION()
	void HandleUnhovered();

	UFUNCTION()
	void HandlePressed();

	UFUNCTION()
	void HandleReleased();

	//아트를 쓰는 버튼인지, 텍스처가 하나라도 지정되어 있으면 쓴다
	bool UsesArt() const;

	//아트로 쓸 콘텐츠 이미지, 아트를 쓰지 않거나 콘텐츠가 이미지가 아니면 nullptr
	UImage* ResolveArtImage() const;

	//호버 시 내릴 위젯, 아트 버튼이면 아트가 내려가지 않도록 이름 지정이 있을 때만 돌려준다
	UWidget* ResolveHoverTarget() const;

	//상태 전환의 단일 출처, 아트·라벨 오프셋·통지를 함께 처리한다
	void SetButtonState(EImageButtonState state);

	UTexture2D* TextureForState(EImageButtonState state) const;

	//아트 교체 및 네이티브 크기 × 배율 적용, 텍스처가 없으면 유지
	void ApplyArt(UTexture2D* texture);

	void ApplyHoverOffset(bool bHovered);

	EImageButtonState currentState = EImageButtonState::Normal;

	//호버 전 라벨의 렌더 트랜슬레이션, 디자이너가 넣어 둔 값을 0으로 덮지 않도록 보관한다
	FVector2D baseTranslation = FVector2D::ZeroVector;
	bool bBaseTranslationCached = false;
};