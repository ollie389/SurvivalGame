#include "LyraActorWithAbilitiesAndHealth.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/LyraCombatSet.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Character/LyraHealthComponent.h"


const FName ALyraActorWithAbilitiesAndHealth::CombatSetName = TEXT("CombatSet");
const FName ALyraActorWithAbilitiesAndHealth::HealthComponentName = TEXT("HealthComponent");
const FName ALyraActorWithAbilitiesAndHealth::HealthSetName = TEXT("HealthSet");


ALyraActorWithAbilitiesAndHealth::ALyraActorWithAbilitiesAndHealth(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CombatSet = ObjectInitializer.CreateDefaultSubobject<ULyraCombatSet>(this, CombatSetName);
	HealthSet = ObjectInitializer.CreateDefaultSubobject<ULyraHealthSet>(this, HealthSetName);

	HealthComponent = ObjectInitializer.CreateDefaultSubobject<ULyraHealthComponent>(this, HealthComponentName);
}


void ALyraActorWithAbilitiesAndHealth::InitializeAbilitySystem()
{
	// Super::InitializeAbilitySystem is required to get the ASC working, do this first:
	Super::InitializeAbilitySystem();

	// HealthComponent requires a Lyra based ASC
	ULyraAbilitySystemComponent* ASC = GetLyraAbilitySystemComponentChecked();

	// Add Attribute Sets to ASC
	ASC->AddAttributeSetSubobject(CombatSet.Get());
	ASC->AddAttributeSetSubobject(HealthSet.Get());

	// DO NOT init HealthComponent until AFTER HealthSet has been added
	HealthComponent->InitializeWithAbilitySystem(ASC);
}


void ALyraActorWithAbilitiesAndHealth::UninitializeAbilitySystem()
{
	HealthComponent->UninitializeFromAbilitySystem();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->RemoveSpawnedAttribute(CombatSet.Get());
		ASC->RemoveSpawnedAttribute(HealthSet.Get());
	}

	Super::UninitializeAbilitySystem();
}