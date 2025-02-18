// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCPPClasses/Abilities/LyraGamePlayAbility_Consumeable.h"
#include "Character/LyraCharacter.h"
#include "CustomCPPClasses/Character/LyraPlayerCharacter.h"
#include "Inventory/LyraInventoryManagerComponent.h"
#include "Player/LyraPlayerController.h"
#include "Inventory/LyraInventoryItemInstance.h"
#include "Equipment/LyraQuickBarComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"


void ULyraGamePlayAbility_Consumeable::ALyraGamePlayAbility_Consumeable() {
	CurrentAmmoAbility = false;
	InputAction = NULL;
}

void ULyraGamePlayAbility_Consumeable::SetWeaponActor() {
	ALyraPlayerCharacter* LyraPlayerCharacter = Cast<ALyraPlayerCharacter>(GetLyraCharacterFromActorInfo());
	EquippedWeaponItemInstance = 
		LyraPlayerCharacter->GetController()->GetComponentByClass<ULyraQuickBarComponent>()->GetActiveSlotItem();
}


void ULyraGamePlayAbility_Consumeable::ReceivedItemCallback(FGameplayTag InChannel, const FAbilityItemsArray& InMessage) {
	for (FAbilityItems AbilityItem : InMessage.AbilityItemsArray) {
		if (AbilityItem.ItemDefinition == RequiredItems[0].ItemDefinition) {
			int AddStack = AbilityItem.ConsumptionCount;
			if (AddStack > 0) {
				EquippedWeaponItemInstance->
					AddStatTagStack(FGameplayTag::RequestGameplayTag(FName("Lyra.ShooterGame.Weapon.MagazineAmmo")), AddStack);
			}
			else {
				EquippedWeaponItemInstance->
					RemoveStatTagStack(FGameplayTag::RequestGameplayTag(FName("Lyra.ShooterGame.Weapon.MagazineAmmo")), -AddStack);
			}

		}
	}
	//if (InMessage.ItemDefinition) == 
}


void ULyraGamePlayAbility_Consumeable::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	InventoryManager = GetLyraPlayerControllerFromActorInfo()->GetComponentByClass<ULyraInventoryManagerComponent>();

	if (InputAction) {
		ULyraAbilitySystemComponent* localASC = GetLyraAbilitySystemComponentFromActorInfo();
		localASC->SetInputBinding(InputAction, Spec.Handle);
	}

	if (CurrentAmmoAbility) {
		SetWeaponActor();
		if (RequiredItems.Num()) {
			TSubclassOf<ULyraInventoryItemDefinition> RequiredItem = RequiredItems[0].ItemDefinition;
			int AvailableStacks = 0;
			InventoryItems = InventoryManager->GetAllItems();
			for (auto Item : InventoryItems) {
				if (Item->GetItemDef() == RequiredItem) {
					AvailableStacks += Item->GetStatTagStackCount(
						FGameplayTag::RequestGameplayTag(FName("Inventory.Item.StackCount")));
				}
			}
			EquippedWeaponItemInstance->
				AddStatTagStack(FGameplayTag::RequestGameplayTag(FName("Lyra.ShooterGame.Weapon.MagazineAmmo")), AvailableStacks);
		}

		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
		FGameplayTag ChannelTag; // You will need to assign this value as appropriate for your code
		ChannelTag = FGameplayTag::RequestGameplayTag(FName("Inventory.Item.ReceivedItem"));
		MessageSubsystem.RegisterListener(ChannelTag, this, &ULyraGamePlayAbility_Consumeable::ReceivedItemCallback);
	}
}

bool ULyraGamePlayAbility_Consumeable::HasItemsForAbility() 
{
	InventoryItems = InventoryManager->GetAllItems();

	for (auto Item : RequiredItems)
	{
		if (!DoWeHaveEnough(InventoryItems, Item.ItemDefinition, Item.ConsumptionCount)) {
			return false;
		}
	}
	return true;
}


bool ULyraGamePlayAbility_Consumeable::DoWeHaveEnough(const TArray<ULyraInventoryItemInstance*> InventoryItemsParam, const TSubclassOf<ULyraInventoryItemDefinition>ItemDef, const int Needed) 
{
	int TempCount = 0;
	for (auto Item : InventoryItemsParam)
	{
		if (!Item->GetStatTagStackCount(FGameplayTag::RequestGameplayTag(FName("Inventory.Item.InStorage"))) && Item->GetItemDef() == ItemDef)
		{
			TempCount += Item->GetStatTagStackCount(FGameplayTag::RequestGameplayTag(FName("Inventory.Item.StackCount")));
			if (TempCount >= Needed) return true;
		}
	}
	return false;
}


void ULyraGamePlayAbility_Consumeable::AdjustInventoryStack(const TArray<ULyraInventoryItemInstance*> InventoryItemsParam, const TSubclassOf<ULyraInventoryItemDefinition>ItemDef, const int Stacks)
{
	if (CurrentActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Authority)
	{
		int StacksToProcess = Stacks;
		int AvailableStacks = 0;

		for (auto Item : InventoryItemsParam)
		{
			if (!Item->GetStatTagStackCount(FGameplayTag::RequestGameplayTag(FName("Inventory.Item.InStorage"))) && Item->GetItemDef() == ItemDef)
			{
				AvailableStacks = Item->GetStatTagStackCount(FGameplayTag::RequestGameplayTag(FName("Inventory.Item.StackCount")));

				if (CurrentAmmoAbility) {
					EquippedWeaponItemInstance->
						RemoveStatTagStack(FGameplayTag::RequestGameplayTag(FName("Lyra.ShooterGame.Weapon.MagazineAmmo")), StacksToProcess);
				}

				if (AvailableStacks > StacksToProcess)
				{
					Item->RemoveStatTagStack(FGameplayTag::RequestGameplayTag(FName("Inventory.Item.StackCount")), StacksToProcess);
					return;
				}
				else if (AvailableStacks == StacksToProcess)
				{
					InventoryManager->RemoveItemInstance(Item);
					return;
				}
				else
				{
					InventoryManager->RemoveItemInstance(Item);
					StacksToProcess -= AvailableStacks;
				}
			}
		}
	}
}


void ULyraGamePlayAbility_Consumeable::ConsumeItems()
{
	for (auto Item : RequiredItems)
	{
		AdjustInventoryStack(InventoryItems, Item.ItemDefinition, Item.ConsumptionCount);
	}
}