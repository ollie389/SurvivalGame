#include "NPC/NPCSpawner.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "AbilitySystem/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnData.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "GameFramework/PlayerState.h"
#include "GameModes/LyraExperienceManagerComponent.h"
#include "Teams/LyraTeamSubsystem.h"
#include "Components/HierarchicalInstancedStaticMeshComponent.h"
#include "CustomCPPClasses/Character/LyraPlayerCharacter.h"
#include "Engine.h"


ANPCSpawner::ANPCSpawner()
{
	PrimaryActorTick.bCanEverTick = false;
	SpawnHeightOffset = 0.0f;
	Lifespan = 0.0f;
	CheckDestroyNPCInterval = 5.0f;
	DespawnCheckRadius = 10000;
}

// Called when the game starts or when spawned
void ANPCSpawner::BeginPlay()
{
	Super::BeginPlay();

	// Delayed for the sake of the cosmetics components added in the experience

	// Listen for the experience load to complete
	const AGameStateBase* GameState = GetWorld()->GetGameState();
	check(GameState);

	ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
	check(ExperienceComponent);
	ExperienceComponent->CallOrRegister_OnExperienceLoaded_LowPriority(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	SetLifeSpan(Lifespan);
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("CheckIfShouldDespawnNPC"), this);
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, CheckDestroyNPCInterval, true);
}

void ANPCSpawner::OnExperienceLoaded(const ULyraExperienceDefinition* Experience)
{
#if WITH_SERVER_CODE
	if (HasAuthority())
	{
		ServerCreateNPCs();
	}
#endif
}

void ANPCSpawner::ServerCreateNPCs()
{
	if (ControllerClass == nullptr)
	{
		return;
	}

	// Create them
	for (int32 Count = 0; Count < NumNPCToCreate; ++Count)
	{
		SpawnOneNPC();
	}
}

// similar to UAIBlueprintHelperLibrary::SpawnAIFromClass but we use the controller class defined here instead of the one set on the pawn
// #todo could make a new static function in  UAIBlueprintHelperLibrary, like SpawnAIFromClassSpecifyController
APawn* ANPCSpawner::SpawnAIFromClass(UObject* WorldContextObject, ULyraPawnData* LoadedPawnData, UBehaviorTree* BehaviorTreeToRun, FVector Location, FRotator Rotation, bool bNoCollisionFail, AActor* PawnOwner, TSubclassOf
	<AAIController> ControllerClassToSpawn)
{
	// GetWorld(), LoadedPawnData->PawnClass, BehaviorTree, GetActorLocation(), GetActorRotation(), true, this, ControllerClass

	APawn* NewPawn = NULL;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World && *LoadedPawnData->PawnClass)
	{
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.Owner = PawnOwner;
		ActorSpawnParams.ObjectFlags |= RF_Transient;	// We never want to save spawned AI pawns into a map
		ActorSpawnParams.SpawnCollisionHandlingOverride = bNoCollisionFail ? ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn : ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		// defer spawning the pawn to setup the AIController, else it spawns the default controller on spawn if set to spawn AI on spawn
		ActorSpawnParams.bDeferConstruction = ControllerClassToSpawn != nullptr;

		NewPawn = World->SpawnActor<APawn>(*LoadedPawnData->PawnClass, Location, Rotation, ActorSpawnParams);
		if (ControllerClassToSpawn)
		{
			NewPawn->AIControllerClass = ControllerClassToSpawn;
			if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(NewPawn))
			{
				PawnExtComp->SetPawnData(LoadedPawnData);
			}
			NewPawn->FinishSpawning(FTransform(Rotation, Location, GetActorScale3D()));
		}

		if (NewPawn != NULL)
		{
			if (NewPawn->Controller == NULL)
			{
				// NOTE: SpawnDefaultController ALSO calls Possess() to possess the pawn (if a controller is successfully spawned).
				NewPawn->SpawnDefaultController();

				if (APlayerState* PlayerState = NewPawn->Controller->PlayerState)
				{
					PlayerState->SetPlayerName("NPC Pawn"); // #todo get the name from PawnData maybe or other ways
				}

			}

			if (BehaviorTreeToRun != NULL)
			{
				AAIController* AIController = Cast<AAIController>(NewPawn->Controller);

				if (AIController != NULL)
				{
					AIController->RunBehaviorTree(BehaviorTreeToRun);
				}
			}
		}
	}

	return NewPawn;
}

void ANPCSpawner::OnSpawnedPawnDestroyed(AActor* DestroyedActor)
{
	if (!HasAuthority())
	{
		return;
	}

	//#todo remove from the SpawnedNPCList list correctly

	if (ShouldRespawn)
	{
		FTimerHandle RespawnHandle;
		GetWorldTimerManager().SetTimer(RespawnHandle, this, &ThisClass::SpawnOneNPC, RespawnTime, false);
	}
	else
	{
		this->Destroy();
	}

}

void ANPCSpawner::SpawnOneNPC()
{
	ULyraPawnData* LoadedPawnData = PawnData.Get();
	if (!PawnData.IsValid())
	{
		LoadedPawnData = PawnData.LoadSynchronous();
	}

	if (LoadedPawnData)
	{
		if (APawn* SpawnedNPC = SpawnAIFromClass(GetWorld(), LoadedPawnData, BehaviorTree, GetActorLocation()+FVector(0,0,SpawnHeightOffset), GetActorRotation(), true, this, ControllerClass))
		{
			bool bWantsPlayerState = true;
			if (const AAIController* AIController = Cast<AAIController>(SpawnedNPC->Controller))
			{
				bWantsPlayerState = AIController->bWantsPlayerState;
			}

			if (ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(SpawnedNPC))
			{
				AActor* AbilityOwner = bWantsPlayerState ? SpawnedNPC->GetPlayerState() : Cast<AActor>(SpawnedNPC);

				if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AbilityOwner))
				{
					PawnExtComp->InitializeAbilitySystem(Cast<ULyraAbilitySystemComponent>(AbilitySystemComponent), AbilityOwner);
				}
			}

			if (ULyraTeamSubsystem* TeamSubsystem = UWorld::GetSubsystem<ULyraTeamSubsystem>(GetWorld()))
			{
				TeamSubsystem->ChangeTeamForActor(SpawnedNPC->Controller, TeamID);
			}

			SpawnedNPCList.Add(Cast<AAIController>(SpawnedNPC->Controller));

			SpawnedNPC->OnDestroyed.AddDynamic(this, &ThisClass::OnSpawnedPawnDestroyed);

			OnAIPawnSpawned(SpawnedNPC);
		}
	}
}

void ANPCSpawner::CheckIfShouldDespawnNPC()
{
	if (HasAuthority()) {
		// Create the TArray of object type queries
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		TArray<AActor*> ActorsToIgnore;

		for (AAIController* NPCController : SpawnedNPCList)
		{
			APawn* NPC = NPCController->GetPawn();
			if (NPC) {
				TArray<AActor*> OutActors;
				ActorsToIgnore.Add(NPC);
				UKismetSystemLibrary::SphereOverlapActors(GetWorld(), NPC->GetActorLocation(), DespawnCheckRadius, ObjectTypes, ALyraPlayerCharacter::StaticClass(), ActorsToIgnore, OutActors);
				if (OutActors.Num() == 0)
				{
					FTransform NPCTransform = GetActorTransform();
					NPC->Destroy();
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Destroying NPC"));

					if (HISMRef)
					{
						HISMRef->AddInstance(NPCTransform, true);
					}

					SetLifeSpan(5.0f);
				}
			}
		}
	}
}