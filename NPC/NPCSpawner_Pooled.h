#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GameFramework/Actor.h"
#include "GameModes/LyraExperienceDefinition.h"
#include "PoolManager/Public/PoolManagerBPLibrary.h"
#include "PoolManager/Public/PoolActorInterface.h"
#include "NPCSpawner_Pooled.generated.h"

class AAIController;
class ULyraPawnData;
class UHierarchicalInstancedStaticMeshComponent;

UCLASS()
class LYRAGAME_API ANPCSpawner_Pooled : public AActor, public IPoolActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANPCSpawner_Pooled();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	TSoftObjectPtr<ULyraPawnData> PawnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FGenericTeamId TeamID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawning)
	float SpawnHeightOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lifespan)
	float Lifespan;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Lifespan)
	float CheckDestroyNPCInterval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	TSubclassOf<AActor> PlayerCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Pawn)
	float DespawnCheckRadius;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void OnExperienceLoaded(const ULyraExperienceDefinition* Experience);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn, meta = (UIMin = 1))
	int32 NumNPCToCreate = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	bool ShouldRespawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn)
	UHierarchicalInstancedStaticMeshComponent* HISMRef;

	/**
	 * Time it takes after pawn death to spawn a new pawn
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Spawn, meta = (EditCondition = ShouldRespawn, EditConditionHides))
	float RespawnTime = 1.f;

	UPROPERTY(EditAnywhere, Category = Gameplay)
	TSubclassOf<AAIController> ControllerClass;

	/** #todo find out how to pool the AIControllers. At the moment the controllers are destroyed with APawn::DetachFromControllerPendingDestroy() **/
	UPROPERTY(Transient)
	TArray<TObjectPtr<AAIController>> SpawnedNPCList;

	virtual void ServerCreateNPCs();

	APawn* SpawnAIFromClass(UObject* WorldContextObject, ULyraPawnData* LoadedPawnData, UBehaviorTree* BehaviorTreeToRun,
		FVector Location,
		FRotator Rotation, bool bNoCollisionFail, AActor* PawnOwner, TSubclassOf<AAIController> ControllerClassToSpawn);

	UFUNCTION(BlueprintImplementableEvent)
	void OnAIPawnSpawned(APawn* SpawnedPawn);

	UFUNCTION()
	void OnSpawnedPawnDestroyed(AActor* DestroyedActor);

	virtual void SpawnOneNPC();

	UFUNCTION()
	void CheckIfShouldDespawnNPC();

	// Called right after spawning or fetching from pool
	virtual void OnPoolBegin_Implementation() override;

	// Called right after the actor is released to the pool
	virtual void OnPoolEnd_Implementation() override;

	UFUNCTION ()
	void DestroySelf();

	UPROPERTY()
	int Released;

	UFUNCTION (BlueprintCallable, BlueprintImplementableEvent)
	void ReleaseChild();
};