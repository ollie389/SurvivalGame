// Copyright 2019 Island Dreaming Studios


#include "LyraGameplayAbility_Persistent.h"
#include "Player/LyraPlayerController.h"
#include "Inventory/LyraInventoryItemInstance.h"
#include "Equipment/LyraQuickBarComponent.h"
#include "CustomCPPClasses/Inventory/InventoryFragment_InventoryIcon.h"
#include "Inventory/InventoryFragment_QuickBarIcon.h"
#include "Inventory/LyraInventoryManagerComponent.h"
#include "LyraWorldCollectablePlus.h"  // Contains the Fragment Payload definition

#include "GameFramework/GameplayMessageSubsystem.h"

void ULyraGameplayAbility_Persistent::PersistInventory()
{
	ALyraPlayerController* PlayerController = GetLyraPlayerControllerFromActorInfo();
	UActorComponent* ActorComponent = PlayerController->GetComponentByClass(ULyraInventoryManagerComponent::StaticClass());
	ULyraInventoryManagerComponent* LyraInventoryManagerComponent = Cast<ULyraInventoryManagerComponent>(ActorComponent);
	if (LyraInventoryManagerComponent)
	{
		//LyraInventoryManagerComponent->UpdatePersistentInventory();
	}
	else
	{
		//UE_LOG(LogPersistence, Error, TEXT("Persistent inventory failed in the ability system."));
	}
}

void ULyraGameplayAbility_Persistent::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Push the current state of the inventory to the database
	PersistInventory();

	// Then end as normal
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void ULyraGameplayAbility_Persistent::LimitStack(int32 MaxStack, int32 CurrentCount, int32 RequestAdd, int32& AdjustedCount, int32& Remainder)
{
	if (MaxStack >= (CurrentCount + RequestAdd))
	{
		AdjustedCount = RequestAdd;
		Remainder = 0;
		return;
	}
	else
	{
		AdjustedCount = MaxStack - CurrentCount;
		Remainder = RequestAdd - AdjustedCount;
		return;
	}
}

void ULyraGameplayAbility_Persistent::CheckAllowedStacking(ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed, int32& MaxStack)
{
	FGameplayTag Tag;
	if (InventoryItemInstance && InventoryItemInstance->GetStatTagStackCount(Tag) > 1)
	{
		MaxStack = InventoryItemInstance->GetStatTagStackCount(Tag);
		bAllowed = true;
		return;

	}
	else
	{
		MaxStack = 0;
		bAllowed = false;
		return;
	}
}

void ULyraGameplayAbility_Persistent::CheckAllowedDelete(ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed)
{
	// Check that the inventory instance is valid, if not return not allowed.
	if (!InventoryItemInstance)
	{
		bAllowed = false;
		return;
	}

	// Get the fragment and the flag to determine if it is allowed.
	const UInventoryFragment_InventoryIcon* tempFragment = InventoryItemInstance->FindFragmentByClass<UInventoryFragment_InventoryIcon>();
	if (tempFragment)
	{
		bAllowed = tempFragment->bCanBeDeleted;
		return;
	}

	bAllowed = false;
}

void ULyraGameplayAbility_Persistent::CheckAllowedDrop(ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed)
{
	// Check that the inventory instance is valid, if not return not allowed.
	if (!InventoryItemInstance)
	{
		bAllowed = false;
		return;
	}

	// Get the fragment and the flag to determine if it is allowed.
	const UInventoryFragment_InventoryIcon* tempFragment = InventoryItemInstance->FindFragmentByClass<UInventoryFragment_InventoryIcon>();
	if (tempFragment)
	{
		bAllowed = tempFragment->bCanBeDropped;
		return;
	}

	bAllowed = false;

}

void ULyraGameplayAbility_Persistent::CheckAllowedStorage(ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed)
{
	// Check that the inventory instance is valid, if not return not allowed.
	if (!InventoryItemInstance)
	{
		bAllowed = false;
		return;
	}

	// Get the fragment and the flag to determine if it is allowed.
	const UInventoryFragment_InventoryIcon* tempFragment = InventoryItemInstance->FindFragmentByClass<UInventoryFragment_InventoryIcon>();
	if (tempFragment)
	{
		bAllowed = tempFragment->bCanBeStored;
		return;
	}

	bAllowed = false;

}

ULyraInventoryManagerComponent* ULyraGameplayAbility_Persistent::GetInventoryManager(bool& bIsValid)
{

	if (!GetLyraPlayerControllerFromActorInfo())
	{
		bIsValid = false;
		return nullptr;
	}

	ULyraInventoryManagerComponent* InventoryManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<ULyraInventoryManagerComponent>();
	if (InventoryManager)
	{
		bIsValid = true;
		return InventoryManager;
	}

	bIsValid = false;
	return nullptr;
}



ULyraQuickBarComponent* ULyraGameplayAbility_Persistent::GetQuickBarManager(bool& bIsValid)
{
	if (GetLyraPlayerControllerFromActorInfo())
	{
		bIsValid = false;
		return nullptr;
	}

	ULyraQuickBarComponent* QuickBar = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<ULyraQuickBarComponent>();
	if (QuickBar)
	{
		bIsValid = true;
		return QuickBar;
	}

	bIsValid = false;
	return nullptr;
}

void ULyraGameplayAbility_Persistent::BroadcastRefreshGrids(bool bBroadcastStandardRefresh, bool bBroadcastStorageRefresh)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	//UGameplayMessageSubsystem* MessageSubsystem = UGameInstance::GetSubsystem<UGameplayMessageSubsystem>(UGameplayStatics::GetGameInstance(this));

	if (bBroadcastStandardRefresh && MessageSubsystem.IsValidLowLevelFast())
	{
		FGameplayTag Channel = FGameplayTag::RequestGameplayTag(TEXT("UI.Action.Inventory.Refresh.Standard"));
		FLyraFragmentPayload Message;

		// Set up the struct data
		Message.InventoryGridType = FGameplayTag::RequestGameplayTag(TEXT("UI.Action.Inventory.Grid.Standard"));

		// Send the broadcast
		MessageSubsystem.BroadcastMessage(Channel, Message);
	}

	if (bBroadcastStorageRefresh && MessageSubsystem.IsValidLowLevelFast())
	{
		FGameplayTag Channel = FGameplayTag::RequestGameplayTag(TEXT("UI.Action.Inventory.Refresh.Storage"));
		FLyraFragmentPayload Message;

		// Set up the struct data
		Message.InventoryGridType = FGameplayTag::RequestGameplayTag(TEXT("UI.Action.Inventory.Grid.Storage"));

		// Send the broadcast
		MessageSubsystem.BroadcastMessage(Channel, Message);
	}
}

void ULyraGameplayAbility_Persistent::ParsePayloadOneItem(FGameplayEventData Payload, bool bCommitAbility, bool& isValid, ULyraInventoryItemInstance*& InventoryItemInstance)
{
	// If requested, commit this ability
	if (bCommitAbility)
	{
		FGameplayTagContainer OptionalRelevantTags = FGameplayTagContainer(); // OUT
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, &OptionalRelevantTags);
	}

	// Convert the optionalobject passed via the payload into an inventory item instance
	const UObject* obj = Payload.OptionalObject;
	UObject* obj_nonconst = const_cast<UObject*>(obj);
	InventoryItemInstance = Cast<ULyraInventoryItemInstance>(obj_nonconst);
	if (InventoryItemInstance)
	{
		isValid = true;
	}
	else
	{
		isValid = false;
	}

}

void ULyraGameplayAbility_Persistent::ParsePayloadTwoItems(FGameplayEventData Payload, bool bCommitAbility, bool& isValid, ULyraInventoryItemInstance*& ToInventoryItemInstance, ULyraInventoryItemInstance*& FromInventoryItemInstance)
{
	// If requested, commit this ability
	if (bCommitAbility)
	{
		FGameplayTagContainer OptionalRelevantTags = FGameplayTagContainer(); // OUT
		CommitAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, &OptionalRelevantTags);
	}

	// First check that our from item is valid
	const UObject* obj = Payload.OptionalObject;
	UObject* obj_nonconst = const_cast<UObject*>(obj);
	FromInventoryItemInstance = Cast<ULyraInventoryItemInstance>(obj_nonconst);
	if (FromInventoryItemInstance)
	{
		// then check the To item
		const UObject* obj2 = Payload.OptionalObject2;
		UObject* obj2_nonconst = const_cast<UObject*>(obj2);
		ToInventoryItemInstance = Cast<ULyraInventoryItemInstance>(obj2_nonconst);
		if (ToInventoryItemInstance)
		{
			isValid = true; // both are valid
		}
		else
		{
			isValid = false;
		}
	}
	else
	{
		isValid = false;
	}




}

void ULyraGameplayAbility_Persistent::RemoveItemFromQuickBar(ULyraInventoryItemInstance* TargetInventoryItemInstance)
{
	// Is the target even valid?
	if (TargetInventoryItemInstance)
	{
		// Can the target even be in a quick slot?
		const UInventoryFragment_QuickBarIcon* QuickBarIconFragment = TargetInventoryItemInstance->FindFragmentByClass<UInventoryFragment_QuickBarIcon>();
		if (QuickBarIconFragment)
		{
			// Loop the quick bar looking for this target in any of the slots.
			bool bIsValid;
			ULyraQuickBarComponent* QuickBar = GetQuickBarManager(bIsValid);

			// for (ULyraInventoryItemInstance* InventoryItemInstance : QuickBar->GetSlots())  // Can't use this method as we need the slot index to remove the item.
			// instead we need to loop the array
			TArray<ULyraInventoryItemInstance*> Slots;
			Slots = QuickBar->GetSlots();
			for (int32 i = 0; i < Slots.Num(); i++)
			{

				if (Slots[i] == TargetInventoryItemInstance)
				{
					// Found this target in a slot.  We are going to remove it.
					// first check - if this slot is the active slot we need to cycle the slot forward before removing it.
					if (QuickBar->GetActiveSlotItem() == TargetInventoryItemInstance)
					{
						QuickBar->CycleActiveSlotForward();
					}

					// now remove it.
					QuickBar->RemoveItemFromSlot(i);
				}
			}
		}
	}

}


