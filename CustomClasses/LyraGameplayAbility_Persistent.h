// Copyright 2019 Island Dreaming Studios

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/LyraGameplayAbility.h"
#include "LyraGameplayAbility_Persistent.generated.h"


/**
 * This class is a generic helper class that contains functions that are needed across many of the individual inventory related abilities
 *
 * Two changes are required to the base code to allow for linking (Error LNK2019 Errors).
 * add LYRAGAME_API to the header file for the QuickBar and QuickBarIcon in LyraGame
 * i.e. class LYRAGAME_API UInventoryFragment_QuickBarIcon : public ULyraInventoryItemFragment
 * i.e. class LYRAGAME_API ULyraQuickBarComponent : public UControllerComponent
 */
UCLASS()
class LYRAGAME_API ULyraGameplayAbility_Persistent : public ULyraGameplayAbility
{
	GENERATED_BODY()

protected:

	// Stores the current state of the inventory to the persistent layer
	void PersistInventory();

	/** Native function, called if an ability ends normally or abnormally. If bReplicate is set to true, try to replicate the ending to the client/server */
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	/* Determines if the requested quantity can be added to the item and adjusts accordingly */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void LimitStack(int32 MaxStack, int32 CurrentCount, int32 RequestAdd, int32& AdjustedCount, int32& Remainder);

	/* Determines if the item is configured for stacking */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CheckAllowedStacking(class ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed, int32& MaxStack);

	/* Determine if the item can be deleted from inventory */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CheckAllowedDelete(class ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed);

	/* Determine if the item can be dropped into the world */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CheckAllowedDrop(class ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed);

	/* Determine if the item can be added to storage */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void CheckAllowedStorage(class ULyraInventoryItemInstance* InventoryItemInstance, bool& bAllowed);

	/* Helper function to get the inventory manager reference, should make this a pure function */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	ULyraInventoryManagerComponent* GetInventoryManager(bool& bIsValid);

	/* Helper function to get the quick bar reference, should make this a pure function */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	ULyraQuickBarComponent* GetQuickBarManager(bool& bIsValid);

	/* Helper function to broadcast updates to the inventory grids*/
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void BroadcastRefreshGrids(bool bBroadcastStandardRefresh, bool bBroadcastStorageRefresh);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ParsePayloadOneItem(FGameplayEventData Payload, bool bCommitAbility, bool& isValid, class ULyraInventoryItemInstance*& InventoryItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ParsePayloadTwoItems(FGameplayEventData Payload, bool bCommitAbility, bool& isValid, class ULyraInventoryItemInstance*& ToInventoryItemInstance, class ULyraInventoryItemInstance*& FromInventoryItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RemoveItemFromQuickBar(class ULyraInventoryItemInstance* TargetInventoryItemInstance);



};
