
// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Interaction/IInteractableTarget.h"
#include "Interaction/InteractionOption.h"
#include "Inventory/IPickupable.h"
#include "UObject/UObjectGlobals.h"

#include "LyraWorldCollectablePlus.generated.h"

class UObject;
struct FInteractionQuery;

/** A message when you need to pass a fragment payload through a message */
USTRUCT(BlueprintType)
struct FLyraFragmentPayload
{
	GENERATED_BODY()

	// Single fragment payload
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<class ULyraInventoryItemFragment> Fragment = nullptr;

	// Multiple fragments payload
	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<class ULyraInventoryItemFragment>> Fragments;

	// Tag for the related grid, Enables multi-grid filtering through the payload
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag InventoryGridType;

	// Text to display above the grid when this filter is being applied
	UPROPERTY(BlueprintReadWrite)
	FText Title;


};


/**
 *
 */
UCLASS(Abstract, Blueprintable)
class LYRAGAME_API ALyraWorldCollectablePlus : public AActor, public IInteractableTarget, public IPickupable
{
	GENERATED_BODY()

public:

	ALyraWorldCollectablePlus();

	// Epic Code
	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;

	// Epic Code
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	virtual FInventoryPickup GetPickupInventory() const override;

	// Enables drop to set the pickup inventory (our code)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SetPickupInventory(const FInventoryPickup& newInventoryPickup);

	// Enables a pickup to be activated/deactivated (our code)
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool IsActive();

	// Enables a pickup to be activated/deactivated (our code)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Activate();

	// Enables a pickup to be activated/deactivated (our code)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Deactivate();

	// Enables punch-out to blueprint for Inteact (our code)
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
	void OnInteract();

	// Enables punch-out to blueprint for Activated (our code)
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnActivated();

	// Enables punch-out to blueprint for Deactivated (our code)
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void OnDeactivated();


protected:
	// From Epic Code
	UPROPERTY(EditAnywhere)
	FInteractionOption Option;

	// From Epic Code
	UPROPERTY(EditAnywhere)
	FInventoryPickup StaticInventory;

	// Enables a pickup to be activated/deactivated (our code)
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_ActivationChanged)
	bool bActive = true;

	// // Enables a pickup to be activated/deactivated (our code)
	UFUNCTION()
	void OnRep_ActivationChanged();


};
