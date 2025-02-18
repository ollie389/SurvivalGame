// Fill out your copyright notice in the Description page of Project Settings.


#include "LyraActorWithAbilities.h"

#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "AbilitySystem/LyraAbilitySet.h"

const FName ALyraActorWithAbilities::AbilitySystemComponentName = TEXT("AbilitySystemComponent");


ALyraActorWithAbilities::ALyraActorWithAbilities(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Must replicate Actor for multiplayer
	bReplicates = true;

	AbilitySystemComponent = ObjectInitializer.CreateDefaultSubobject<ULyraAbilitySystemComponent>(this, AbilitySystemComponentName);
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 100.0f;
}


void ALyraActorWithAbilities::PostInitializeComponents()
{
	// BEFORE PostInit Components:
	InitializeAbilitySystem();

	// Now during PostInit Components, there is a functional ASC for other components to use
	Super::PostInitializeComponents();
}


void ALyraActorWithAbilities::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() && AbilitySetOnSpawn)
	{
		AbilitySetOnSpawn->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandlesOnSpawn);
	}
}


void ALyraActorWithAbilities::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UninitializeAbilitySystem();

	Super::EndPlay(EndPlayReason);
}


void ALyraActorWithAbilities::InitializeAbilitySystem()
{

	// We expect this to have been set in the constructor
	check(IsValid(AbilitySystemComponent));

	// Initialize ASC on this Actor
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}


void ALyraActorWithAbilities::UninitializeAbilitySystem()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAbilities();
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();
		AbilitySystemComponent->ClearActorInfo();

		GrantedHandlesOnSpawn.TakeFromAbilitySystem(AbilitySystemComponent);
	}
}


UAbilitySystemComponent* ALyraActorWithAbilities::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


ULyraAbilitySystemComponent* ALyraActorWithAbilities::GetLyraAbilitySystemComponentChecked() const
{
	check(AbilitySystemComponent);
	return AbilitySystemComponent;
}