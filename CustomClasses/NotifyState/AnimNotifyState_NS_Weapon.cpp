// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotifyState_NS_Weapon.h"

#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "CustomCPPClasses/Character/LyraPlayerCharacter.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotifyState_NS_Weapon)

UAnimNotifyState_NS_Weapon::UAnimNotifyState_NS_Weapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Template = nullptr;
	LocationOffset.Set(0.0f, 0.0f, 0.0f);
	RotationOffset = FRotator(0.0f, 0.0f, 0.0f);
	WeaponMeshComp = nullptr;
}

//void UAnimNotifyState_NS_Weapon::SetupWeaponMeshTest_Implementation() const
//{
//	return;
//}

UFXSystemComponent* UAnimNotifyState_NS_Weapon::SpawnEffect(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const
{
	if (MeshComp)
	{
		// Only spawn if we've got valid params
		if (ValidateParameters(MeshComp))
		{
			return UNiagaraFunctionLibrary::SpawnSystemAttached(Template, MeshComp, SocketName, LocationOffset, RotationOffset, EAttachLocation::KeepRelativeOffset, !bDestroyAtEnd);
		}
	}
	return nullptr;
}

UFXSystemComponent* UAnimNotifyState_NS_Weapon::GetSpawnedEffect(UMeshComponent* MeshComp)
{
	if (MeshComp)
	{
		TArray<USceneComponent*> Children;
		MeshComp->GetChildrenComponents(false, Children);

		if (Children.Num())
		{
			for (USceneComponent* Component : Children)
			{
				if (Component && Component->ComponentHasTag(GetSpawnedComponentTag()))
				{
					if (UFXSystemComponent* FXComponent = CastChecked<UFXSystemComponent>(Component))
					{
						return FXComponent;
					}
				}
			}
		}
	}

	return nullptr;
}

void UAnimNotifyState_NS_Weapon::NotifyBegin(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration)
{
}

void UAnimNotifyState_NS_Weapon::NotifyBegin(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	AActor* WeaponActor;
	if (MeshComp)
	{
		AActor* Owner = MeshComp->GetAttachParentActor();
		if (Owner)
		{
			ALyraPlayerCharacter* LyraChar = Cast<ALyraPlayerCharacter>(Owner);
			if (LyraChar)
			{
				LyraChar->GetWeaponActor(WeaponActor);
				if (WeaponActor)
				{
					WeaponMeshComp = WeaponActor->GetComponentByClass <USkeletalMeshComponent>();
				} 
			}
			if (WeaponMeshComp.IsValid())
			{
				if (UFXSystemComponent* Component = SpawnEffect(WeaponMeshComp.Get(), Animation))
				{
					// tag the component with the AnimNotify that is triggering the animation so that we can properly clean it up
					Component->ComponentTags.AddUnique(GetSpawnedComponentTag());
				}
				Super::NotifyBegin(WeaponMeshComp.Get(), Animation, TotalDuration, EventReference);
			}
		}
	}
}

void UAnimNotifyState_NS_Weapon::NotifyEnd(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation)
{
}

void UAnimNotifyState_NS_Weapon::NotifyEnd(USkeletalMeshComponent* MeshComp, class UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	AActor* WeaponActor;
	if (MeshComp)
	{
		AActor* Owner = MeshComp->GetAttachParentActor();
		if (Owner)
		{
			ALyraPlayerCharacter* LyraChar = Cast<ALyraPlayerCharacter>(Owner);
			if (LyraChar)
			{
				LyraChar->GetWeaponActor(WeaponActor);
				if (WeaponActor)
				{
					WeaponMeshComp = WeaponActor->GetComponentByClass <USkeletalMeshComponent>();
				}
			}
			if (WeaponMeshComp.IsValid())
			{
				if (UFXSystemComponent* FXComponent = GetSpawnedEffect(WeaponMeshComp.Get()))
				{
					// untag the component
					FXComponent->ComponentTags.Remove(GetSpawnedComponentTag());

					// Either destroy the component or deactivate it to have it's active FXSystems finish.
					// The component will auto destroy once all FXSystem are gone.
					if (bDestroyAtEnd)
					{
						FXComponent->DestroyComponent();
					}
					else
					{
						FXComponent->Deactivate();
					}
				}

				Super::NotifyEnd(WeaponMeshComp.Get(), Animation, EventReference);
			}
		}
	}
}

bool UAnimNotifyState_NS_Weapon::ValidateParameters(USkeletalMeshComponent* MeshComp) const
{
	bool bValid = true;

	if (MeshComp)
	{
		if (!Template)
		{
			bValid = false;
		}
		else if (!MeshComp->DoesSocketExist(SocketName) && MeshComp->GetBoneIndex(SocketName) == INDEX_NONE)
		{
			bValid = false;
		}
	}
	else return false;

	return bValid;
}

FString UAnimNotifyState_NS_Weapon::GetNotifyName_Implementation() const
{
	if (Template)
	{
		return Template->GetName();
	}

	return UAnimNotifyState::GetNotifyName_Implementation();
}

