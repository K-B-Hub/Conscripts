// Fill out your copyright notice in the Description page of Project Settings.

#include "Widget/ImageButton.h"

#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#if WITH_EDITOR
#include "TextureCompiler.h"
#endif

TSharedRef<SWidget> UImageButton::RebuildWidget()
{
	TSharedRef<SWidget> built = Super::RebuildWidget();

	//자기 호버·클릭 델리게이트를 자기가 받는다, AddUnique라 재빌드에도 중복되지 않는다
	OnHovered.AddUniqueDynamic(this, &UImageButton::HandleHovered);
	OnUnhovered.AddUniqueDynamic(this, &UImageButton::HandleUnhovered);
	OnPressed.AddUniqueDynamic(this, &UImageButton::HandlePressed);
	OnReleased.AddUniqueDynamic(this, &UImageButton::HandleReleased);

	return built;
}

void UImageButton::SetStateTextures(const FImageButtonArt& InArt)
{
	normalTexture = InArt.normal;
	hoveredTexture = InArt.hovered;
	pressedTexture = InArt.pressed;

	//교체 직후 보이는 그림은 지금 상태를 그대로 따라간다
	SetButtonState(currentState);
}

void UImageButton::SetButtonState(EImageButtonState state)
{
	currentState = state;

	ApplyArt(TextureForState(state));

	//라벨 오프셋은 커서가 위에 있는지만 본다, 누르는 동안도 올라간 상태다
	ApplyHoverOffset(state != EImageButtonState::Normal);

	OnStateChanged.ExecuteIfBound(state);
}

UTexture2D* UImageButton::TextureForState(EImageButtonState state) const
{
	//상태 텍스처가 비어 있으면 노말로 떨어진다, 한 장만 꽂아도 동작한다
	switch (state)
	{
	case EImageButtonState::Hovered:
		return hoveredTexture ? hoveredTexture.Get() : normalTexture.Get();
	case EImageButtonState::Pressed:
		return pressedTexture ? pressedTexture.Get() : normalTexture.Get();
	default:
		return normalTexture.Get();
	}
}

void UImageButton::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	//에디터 미리보기와 런타임 초기 상태 모두 노말로 시작
	SetButtonState(EImageButtonState::Normal);

	if (UImage* art = ResolveArtImage())
	{
		art->SetColorAndOpacity(GetIsEnabled() ? FLinearColor::White : disabledTint);
	}
	else if (UsesArt())
	{
		//텍스처를 꽂았는데 콘텐츠가 이미지가 아니면 아트가 조용히 사라진다
		UE_LOG(LogTemp, Warning, TEXT("[ImageButton] %s: 상태 텍스처가 지정됐지만 콘텐츠가 UImage가 아닙니다"),
			*GetName());
	}
}

void UImageButton::SetIsEnabled(bool bInIsEnabled)
{
	Super::SetIsEnabled(bInIsEnabled);

	//비활성 중에는 호버·클릭이 오지 않으므로 상태를 노말로 되돌린다
	SetButtonState(EImageButtonState::Normal);

	if (UImage* art = ResolveArtImage())
	{
		//아트 버튼은 배경 브러시가 투명이라 Disabled 브러시가 보이지 않는다, 틴트로 대신한다
		art->SetColorAndOpacity(bInIsEnabled ? FLinearColor::White : disabledTint);
	}
}

void UImageButton::HandleHovered()
{
	//누르고 있는 중에 들어온 호버는 클릭 상태를 유지한다
	if (currentState != EImageButtonState::Pressed)
	{
		SetButtonState(EImageButtonState::Hovered);
	}
}

void UImageButton::HandleUnhovered()
{
	SetButtonState(EImageButtonState::Normal);
}

void UImageButton::HandlePressed()
{
	SetButtonState(EImageButtonState::Pressed);
}

void UImageButton::HandleReleased()
{
	//누른 자리에서 뗐으면 커서가 아직 위에 있다
	SetButtonState(IsHovered() ? EImageButtonState::Hovered : EImageButtonState::Normal);
}

bool UImageButton::UsesArt() const
{
	return normalTexture || hoveredTexture || pressedTexture;
}

UImage* UImageButton::ResolveArtImage() const
{
	if (!UsesArt()) return nullptr;

	//아트는 버튼의 콘텐츠다, 그래야 버튼 크기가 아트 크기를 따라간다
	return Cast<UImage>(GetContent());
}

UWidget* UImageButton::ResolveHoverTarget() const
{
	if (hoverTextOffsetY == 0.f) return nullptr;

	if (!hoverTextName.IsNone())
	{
		const UUserWidget* owner = GetTypedOuter<UUserWidget>();
		return owner ? owner->GetWidgetFromName(hoverTextName) : nullptr;
	}

	//아트 버튼은 콘텐츠가 아트다, 중앙에서 자라는 그림을 1px 내리면 상쇄되므로 건너뛴다
	if (UsesArt()) return nullptr;

	return GetContent();
}

void UImageButton::ApplyArt(UTexture2D* texture)
{
	if (!texture) return;

	UImage* art = ResolveArtImage();
	if (!art) return;

	art->SetBrushFromTexture(texture);

	//아트는 버튼을 가리지 않는다, 히트 대상이 되면 버튼 위에서 클릭이 먹히지 않는다
	art->SetVisibility(ESlateVisibility::HitTestInvisible);

#if WITH_EDITOR
	//컴파일 중인 텍스처는 대체 텍스처 크기를 돌려준다(Standalone 첫 화면), 크기를 읽기 전에 기다린다
	if (texture->IsCompiling())
	{
		FTextureCompilingManager::Get().FinishCompilation({ texture });
	}
#endif

	//네이티브 해상도 × 배율로 그린다, 상태별 캔버스 크기 차이가 그대로 살아난다
	const FVector2D nativeSize(texture->GetSizeX(), texture->GetSizeY());
	art->SetDesiredSizeOverride(nativeSize * FMath::Max(textureScale, 0.01f));
}

void UImageButton::ApplyHoverOffset(bool bHovered)
{
	UWidget* target = ResolveHoverTarget();
	if (!target) return;

	if (!bBaseTranslationCached)
	{
		baseTranslation = target->GetRenderTransform().Translation;
		bBaseTranslationCached = true;
	}

	//렌더 트랜슬레이션은 레이아웃을 다시 계산하지 않는다, 패딩으로 밀면 줄 전체가 흔들린다
	target->SetRenderTranslation(baseTranslation + FVector2D(0.f, bHovered ? hoverTextOffsetY : 0.f));
}