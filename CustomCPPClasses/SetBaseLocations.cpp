// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCPPClasses/SetBaseLocations.h"

//#include "../Plugins/Voxel/Source/VoxelGraphCore/Public/BlueprintLibrary/VoxelGraphBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "VoxelDependency.h"
//#include "VoxelLandscape.h"
#include "VoxelState.h"
#include "VoxelRuntime.h"
#include "VoxelMetadata.h"
//#include "VoxelBrushSubsystem.h"
#include "VoxelLayerSubsystem.h"
#include "VoxelDependencyTracker.h"
#include "Sculpt/VoxelSculptSubsystem.h"
#include "VoxelLayerSubsystem.h"

#include "CoreMinimal.h"
#include "VoxelWorld.h"


// Sets default values
ASetBaseLocations::ASetBaseLocations()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	NumTeams = 2;
	BorderChannel = FName("Project.IslandBorder");
	HeightChannel = FName("Project.Height");
	WorldSize = 100000;

}

// Called when the game starts or when spawned
void ASetBaseLocations::BeginPlay()
{
	Super::BeginPlay();

}

void ASetBaseLocations::SetUpBases()
{
	for (int TeamID = 1; TeamID <= NumTeams; TeamID++) {
		float BaseAngle = (TeamID - 1) * (360 / NumTeams);
		FVector BaseDirection = FVector(FMath::Sin(BaseAngle*3.14159/180), FMath::Cos(BaseAngle * 3.14159 / 180), 0);

		Voxel::AsyncTask([this, BaseDirection, TeamID]
			{
				VOXEL_FUNCTION_COUNTER();

				FVoxelTaskScope TaskScope(true);

				double BaseX = 0.0;
				double BaseY = 0.0;
				double BaseZ = 0.0;

				if (VoxelWorld) {
					if (const TSharedPtr<FVoxelRuntime> Runtime = VoxelWorld->GetVoxelRuntime())
					{
						if (const TSharedPtr<FVoxelState> State = Runtime->GetState())
						{
							TVoxelArray<double> PositionsX = {};
							TVoxelArray<double> PositionsY = {};
							for (auto i = 0; i < 20; i++) {
								PositionsX.Push((i * WorldSize / 20) * BaseDirection.X);
								PositionsY.Push((i * WorldSize / 20) * BaseDirection.Y);
							}
							TConstVoxelArrayView<double> X(PositionsX);
							TConstVoxelArrayView<double> Y(PositionsY);
							FVoxelMetadata Metadata;
							Metadata.QueryMaterials();
							Metadata.QueryMetadata(FName("BiomeID"));
							FVoxelMetadataView MetadataView(Metadata);
							/*TVoxelArray<float> Heights = State->GetSubsystem<FVoxelWorldBrushSubsystem>().SampleHeightBrushes(
								FVoxelDependencyTracker::Create({}),
								X,
								Y,
								Metadata);*/
							FVoxelLayerName LayerName = FVoxelLayerName(EVoxelLayerType::Height, FName("Default"));
							TVoxelArray<float> Heights = State->GetSubsystem<FVoxelLayerSubsystem>().SampleHeightLayer(
								LayerName,
								FVoxelDependencyTracker::Create({}),
								X,
								Y,
								Metadata);
							TConstVoxelArrayView<float> BiomeIDArray = Metadata.GetMetadataView(FName("BiomeID"));
							for (int i = 0; i < PositionsX.Num(); i++) {
								if (BiomeIDArray[i] == 0) {
									BaseX = PositionsX[i];
									BaseY = PositionsY[i];
									BaseZ = BaseZ = Heights[i];
									GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FVector(BaseX, BaseY, BaseZ).ToString());
									break;
								}
							}
						}
					}
				}

				return Voxel::GameTask([this, BaseX, BaseY, BaseZ, TeamID]
					{
						SpawnBaseEvent(FVector(BaseX, BaseY, BaseZ), TeamID);
					});
			});

		//for (int TeamID = 1; TeamID <= NumTeams; TeamID++)
		//{
		//	//FVector BaseDirection = SetBaseDirection(TeamID);
		//	//FVector BaseLocation = FVector(0, 0, 0);
		//	//float ChannelValue = 0;
		//	//while ((ChannelValue < 0.5) && (BaseLocation.Size() < WorldSize))
		//	//{
		//	//	UVoxelGraphBlueprintLibrary::QueryChannel(GetWorld(), BorderChannel, BaseLocation, 32767, ChannelValue);
		//	//	BaseLocation = BaseLocation + 1000 * BaseDirection;
		//	//}
		//	//BaseLocations.Add(BaseLocation);
		//	///*FTimerHandle TimerHandle;
		//	//TimerArray.Add(TimerHandle);
		//	//FTimerDelegate TimerDelegate;
		//	//TimerDelegate.BindUFunction(this, FName("SpawnBase"), BaseLocation, TeamID, this);
		//	//GetWorldTimerManager().SetTimer(TimerArray[TeamID-1], TimerDelegate, 1.0f, true);*/
		//	//float BaseZ = 0;
		//	//UVoxelGraphBlueprintLibrary::QueryChannel(GetWorld(), HeightChannel, BaseLocation, 32767, BaseZ);
		//	//SpawnBaseEvent(FVector(BaseLocation.X, BaseLocation.Y, BaseZ), TeamID);
		//}
	}
}

void ASetBaseLocations::SpawnBase(FVector BaseLocation, int TeamID)
{
	FVector Start = FVector(BaseLocation.X, BaseLocation.Y, 20000);
	FVector End = FVector(BaseLocation.X, BaseLocation.Y, -20000);
	FHitResult OutHit;
	UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, VoxelTerrainObjectType, false, TArray<AActor*, FDefaultAllocator>(), EDrawDebugTrace::ForDuration, OutHit, true);
	if (HasAuthority())
	{
		if (OutHit.bBlockingHit)
		{
			UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), Start, End, VoxelTerrainObjectType, false, TArray<AActor*, FDefaultAllocator>(), EDrawDebugTrace::Persistent, OutHit, true);
			GetWorldTimerManager().ClearTimer(TimerArray[TeamID - 1]);
			//SpawnBaseEvent(OutHit, TeamID);
		}
	}
	
}

// Called every frame
void ASetBaseLocations::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FVector ASetBaseLocations::SetBaseDirection(int TeamID)
{
	FVector Output = FVector(0, 1, 0);
	//FRotator Direction = FRotator(0, (TeamID - 1) * (360 / NumTeams), 0);
	Output = Output.RotateAngleAxis((TeamID - 1) * (360 / NumTeams), FVector(0, 0, 1));
	return Output;
}

