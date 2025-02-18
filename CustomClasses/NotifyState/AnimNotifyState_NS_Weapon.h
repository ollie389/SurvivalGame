// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UObject/ObjectMacros.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameFramework/Actor.h"
#include "AnimNotifyState_NS_Weapon.generated.h"


class UMeshComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;
class UFXSystemAsset;
class UFXSystemComponent;
class UAnimInstance;

// Timed Niagara Effect Notify
// Allows a looping Niagara effect to be played in an animation that will activate
// at the beginning of the notify and deactivate at the end.
UCLASS(Blueprintable, meta = (DisplayName = "Timed Niagara Effect Weapon"))
class LYRAGAME_API UAnimNotifyState_NS_Weapon : public UAnimNotifyState
{
	GENERATED_UCLASS_BODY()

public:
	// The niagara system template to use when spawning the niagara component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (DisplayName = "Niagara System", ToolTip = "The niagara system to spawn for the notify state"))
	TObjectPtr<UNiagaraSystem> Template;

	//UPROPERTY(EditAnywhere)
	TWeakObjectPtr<USkeletalMeshComponent> WeaponMeshComp;

	// The socket within our mesh component to attach to when we spawn the Niagara component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "The socket or bone to attach the system to", AnimNotifyBoneName = "true"))
	FName SocketName;

	// Offset from the socket / bone location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "Offset from the socket or bone to place the Niagara system"))
	FVector LocationOffset;

	// Offset from the socket / bone rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (ToolTip = "Rotation offset from the socket or bone for the Niagara system"))
	FRotator RotationOffset;

	// Whether or not we destroy the component at the end of the notify or instead just stop
	// the emitters.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = NiagaraSystem, meta = (DisplayName = "Destroy Immediately", ToolTip = "Whether the Niagara system should be immediately destroyed at the end of the notify state or be allowed to finish"))
	bool bDestroyAtEnd;

	UE_DEPRECATED(5.0, "Please use the other NotifyBegin function instead")
		virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration) override;

	UE_DEPRECATED(5.0, "Please use the other NotifyEnd function instead")
		virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation) override;

	virtual void NotifyBegin(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(class USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	// Overridden from UAnimNotifyState to provide custom notify name.
	FString GetNotifyName_Implementation() const override;

	// Return FXSystemComponent created from SpawnEffect
	UFUNCTION(BlueprintCallable, Category = "AnimNotify")
	UFXSystemComponent* GetSpawnedEffect(UMeshComponent* MeshComp);

	//UFUNCTION(BlueprintImplementableEvent, Category = "Setup")
	//void Setup(const AActor* LyraPawn, USkeletalMeshComponent*& WeaponMesh) const;

protected:
	// Spawns the NiagaraSystemComponent. Called from Notify.
	virtual UFXSystemComponent* SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;

	bool ValidateParameters(USkeletalMeshComponent* MeshComp) const;

	FORCEINLINE FName GetSpawnedComponentTag()const { return GetFName(); }
};
