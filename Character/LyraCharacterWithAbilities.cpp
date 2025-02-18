// Copyright Epic Games, Inc. All Rights Reserved.

#include "LyraCharacterWithAbilities.h"

#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "LyraPawnData.h" // @Game-Change 
#include "LyraPawnExtensionComponent.h" // @Game-Change 
#include "AbilitySystem/LyraAbilitySet.h" // @Game-Change 
#include "Components/GameFrameworkComponentManager.h"  // @Game-Change 
#include "Character/LyraHealthComponent.h"
#include "LyraGameplayTags.h"
#include "CustomCPPClasses/Abilities/Attributes/LyraStatComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(LyraCharacterWithAbilities)

// @Game-Change name for the stage when we're ready for the abilities
const FName ALyraCharacterWithAbilities::NAME_LyraAbilityReady("LyraAbilitiesReady");

/* @Game-Change start since the ability system lives here and not on the playerState we want to set the ability set here */
void ALyraCharacterWithAbilities::SetPawnData(const ULyraPawnData* InPawnData)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (InPawnData)
	{
		for (const ULyraAbilitySet* AbilitySet : InPawnData->AbilitySets)
		{

			if (AbilitySet)
			{
				AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
			}
		}

		UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, NAME_LyraAbilityReady);
	}
}

void ALyraCharacterWithAbilities::OnAbilitySystemInitialized()
{


}
/* @Game-Change end since the ability system lives here and not on the playerState we want to set the ability set here */

ALyraCharacterWithAbilities::ALyraCharacterWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULyraAbilitySystemComponent>(this, TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// These attribute sets will be detected by AbilitySystemComponent::InitializeComponent. Keeping a reference so that the sets don't get garbage collected before that.
	HealthSet = CreateDefaultSubobject<ULyraHealthSet>(TEXT("HealthSet"));
	CombatSet = CreateDefaultSubobject<ULyraCombatSet>(TEXT("CombatSet"));

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}

void ALyraCharacterWithAbilities::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	if (AbilitySystemComponent)
	{
		// Add Attribute Sets to ASC
		AbilitySystemComponent->AddAttributeSetSubobject(CombatSet.Get());
		AbilitySystemComponent->AddAttributeSetSubobject(HealthSet.Get());

		AbilitySystemComponent->AddAttributeSetSubobject(StatSet.Get()); //@GameChange

		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
		StatComponent->InitializeWithAbilitySystem(AbilitySystemComponent); //@GameChange
	}

	//Super::OnAbilitySystemInitialized();

	if (const ULyraPawnExtensionComponent* LyraPawnExtensionComponent = ULyraPawnExtensionComponent::FindPawnExtensionComponent(this))
	{
		SetPawnData(LyraPawnExtensionComponent->GetPawnData<ULyraPawnData>());
	}

}

UAbilitySystemComponent* ALyraCharacterWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void ALyraCharacterWithAbilities::OnDeathFinished(AActor* OwningActor)
{
	//@Game-Change override to remove death for pooling

	HealthComponent->DeathState = ELyraDeathState::NotDead;
	AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dead, 0);
	AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Status_Death_Dying, 0);
	K2_OnDeathFinished();
}


void ALyraCharacterWithAbilities::OnDeathStarted(AActor* OwningActor)
{
	//@Game-Change override to remove death for pooling
}

void ALyraCharacterWithAbilities::OnPoolEnd_Implementation() //@GameChange
{
	IPoolActorInterface::OnPoolEnd_Implementation();
	// Setup the actor after getting fetched from the pool
	OnPoolEndEvent.Broadcast(this);
}