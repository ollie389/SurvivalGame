// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Character/LyraCharacter.h"
#include "Components/PrimitiveComponent.h"
#include "PoolManager/Public/PoolActorInterface.h" //@Game-Change
#include "LyraCharacterWithAbilities.generated.h"

class ULyraPawnData;
class UAbilitySystemComponent;
class ULyraAbilitySystemComponent;
class UObject;
class ULyraHealthComponent;
class ULyraStatComponent;

DECLARE_DYNAMIC_MULTICAST_SPARSE_DELEGATE_OneParam(FOnPoolEndSignature, ALyraCharacterWithAbilities, OnPoolEndEvent, AActor*, DestroyedActor); //@Game-Change

// ALyraCharacter typically gets the ability system component from the possessing player state
// This represents a character with a self-contained ability system component.
UCLASS(Blueprintable)
class LYRAGAME_API ALyraCharacterWithAbilities : public ALyraCharacter, public IPoolActorInterface //@Game-Change: Added IPoolActorInterface
{
	GENERATED_BODY()

public:
	FOnPoolEndSignature OnPoolEndEvent; //@Game-Change

	virtual void OnPoolEnd_Implementation() override; //@Game-Change

	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName, const bool bNotifyActor) override
	{
		if (NewBase)
		{
			// LoadClass to not depend on the voxel module
			static UClass* const GraphClass = LoadClass<UObject>(nullptr, TEXT("/Script/VoxelGraphCore.VoxelActor"));
			static UClass* const VoxelWorldClass = LoadClass<UObject>(nullptr, TEXT("/Script/Voxel.VoxelWorld"));

			const AActor* BaseOwner = NewBase->GetOwner();
			if (ensure(GraphClass) &&
				ensure(VoxelWorldClass) &&
				BaseOwner &&
				(BaseOwner->IsA(GraphClass) || BaseOwner->IsA(VoxelWorldClass)))
			{
				NewBase = Cast<UPrimitiveComponent>(BaseOwner->GetRootComponent());
				ensure(NewBase);
			}
		}

		Super::SetBase(NewBase, BoneName, bNotifyActor);
	}

	ALyraCharacterWithAbilities(const FObjectInitializer& ObjectInitializer);

	virtual void PostInitializeComponents() override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	/* @Game-Change start since the ability system lives here and not on the playerState we want to set the ability set here */
	static const FName NAME_LyraAbilityReady;

	void SetPawnData(const ULyraPawnData* InPawnData);



protected:
	virtual void OnAbilitySystemInitialized() override;
	/* @Game-Change end since the ability system lives here and not on the playerState we want to set the ability set here */

	// The ability system component sub-object used by player characters.
	UPROPERTY(VisibleAnywhere, Category = "Lyra|PlayerState")
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	// Health attribute set used by this actor.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lyra|Ability")
	TObjectPtr<class ULyraHealthSet> HealthSet;
	// Combat attribute set used by this actor.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Lyra|Ability")
	TObjectPtr<class ULyraCombatSet> CombatSet;

private:
	virtual void OnDeathFinished(AActor* OwningActor) override; //@Game-Change: Stop NPC death, for pooling
	
	virtual void OnDeathStarted(AActor* OwningActor) override; //@Game-Change: Stop NPC death, for pooling

};
