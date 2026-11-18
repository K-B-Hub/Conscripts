#include "PW.h"
#include "Modules/ModuleManager.h"
#include "Engine/World.h"
#include "NiagaraActor.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, PW, "PW" );

UNiagaraComponent* SpawnFxAtLocation(UWorld* world, UNiagaraSystem* system, const FVector& location, const FRotator& rotation)
{
	if (!world || !system) return nullptr;

	//데칼 프록시는 소유 액터가 숨김이면 게임에서 빠지므로 보이는 액터를 소유자로 둔다
	ANiagaraActor* actor = world->SpawnActor<ANiagaraActor>(location, rotation);
	if (!actor) return nullptr;

	actor->SetDestroyOnSystemFinish(true);
	UNiagaraComponent* comp = actor->GetNiagaraComponent();
	comp->SetAsset(system);
	comp->Activate(true);
	return comp;
}