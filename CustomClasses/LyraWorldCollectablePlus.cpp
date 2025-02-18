// Copyright 2019 Island Dreaming Studios

#include "LyraWorldCollectablePlus.h"
#include "Net/UnrealNetwork.h"

#include "Interaction/IInteractableTarget.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraWorldCollectablePlus)

struct FInteractionQuery;

ALyraWorldCollectablePlus::ALyraWorldCollectablePlus()
{
	bReplicates = true;
}

void ALyraWorldCollectablePlus::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder)
{
	InteractionBuilder.AddInteractionOption(Option);
}

FInventoryPickup ALyraWorldCollectablePlus::GetPickupInventory() const
{
	return StaticInventory;
}

// Standard code does not allow for a run-time update of the inventory, which is needed to make
// dropping inventory work
void ALyraWorldCollectablePlus::SetPickupInventory(const FInventoryPickup& newInventoryPickup)
{
	StaticInventory = newInventoryPickup;
}

// Standard code does not have an active/deactive state, which is needed for nodes that can be depleted
// and reactivated after a period of time 
void ALyraWorldCollectablePlus::Activate()
{
	bActive = true;
	if (HasAuthority()) OnRep_ActivationChanged();
	UE_LOG(LogTemp, Verbose, TEXT("Node Activated"));
}

void ALyraWorldCollectablePlus::Deactivate()
{
	bActive = false;
	if (HasAuthority()) OnRep_ActivationChanged();
	UE_LOG(LogTemp, Verbose, TEXT("Node Deactivated"));
}


bool ALyraWorldCollectablePlus::IsActive()
{
	return bActive;
}

void ALyraWorldCollectablePlus::OnRep_ActivationChanged()
{
	UE_LOG(LogTemp, Verbose, TEXT("Node State Replicated on %s"), (HasAuthority() ? TEXT("server") : TEXT("client")));

	if (bActive)
	{
		OnActivated();
	}
	else
	{
		OnDeactivated();
	}
}

void ALyraWorldCollectablePlus::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALyraWorldCollectablePlus, bActive);

}


