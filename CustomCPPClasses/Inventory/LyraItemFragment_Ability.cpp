// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CustomCPPClasses/Inventory/LyraItemFragment_Ability.h"
#include <AbilitySystem/LyraAbilitySystemComponent.h>
#include "Player/LyraPlayerController.h"
#include "Inventory/LyraInventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraItemFragment_Ability)

void ULyraItemFragment_Ability::OnInstanceCreated(ULyraInventoryItemInstance* Instance) const
{
	ULyraAbilitySystemComponent* localASC = nullptr;

	// The outer returns the owning actor, which in this case is the player control.  
	// I don't love this approach, feels like I am breaking the core design concept
	ALyraPlayerController* localPC = Cast<ALyraPlayerController>(Instance->GetOuter());
	if (localPC)
	{
		localASC = localPC->GetLyraAbilitySystemComponent();
	}

	if (RelatedGameplayAbility && localASC)
	{
		FGameplayAbilitySpecHandle GrantedAbilitySpec = localASC->GiveAbility(FGameplayAbilitySpec(RelatedGameplayAbility, 1));
		//localASC->SetInputBinding(InputAction, GrantedAbilitySpec);
	}
}

