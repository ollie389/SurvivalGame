// Fill out your copyright notice in the Description page of Project Settings.


#include "ProceduralMeshTreeActor.h"
#include "KismetProceduralMeshLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystem/Attributes/LyraHealthSet.h"
#include "Character/LyraHealthComponent.h"
#include "StaticMeshResources.h"

// Sets default values
AProceduralMeshTreeActor::AProceduralMeshTreeActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	CylinderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMeshComponent"));
	CylinderMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CylinderMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CylinderMeshComponent->SetWorldScale3D(FVector(0.3, 0.3, 1));
	CylinderMeshComponent->SetNetAddressable();
	CylinderMeshComponent->SetIsReplicated(true);
	CylinderMeshComponent->SetCanEverAffectNavigation(false);
	CylinderMeshComponent->SetVisibility(false);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	StaticMeshComponent->SetNetAddressable();
	StaticMeshComponent->SetIsReplicated(true);
	StaticMeshComponent->SetCanEverAffectNavigation(false);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	ProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("ProceduralMeshComponent"));
	ProceduralMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CutLeavesProceduralMeshComponent = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("CutLeavesProceduralMeshComponent"));
	CutLeavesProceduralMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	CustomCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CustomCollisionBox"));
	CustomCollisionBox->SetGenerateOverlapEvents(true);
	CustomCollisionBox->OnComponentBeginOverlap.AddDynamic(this, &AProceduralMeshTreeActor::OnCustomCollisionOverlap);
	CustomCollisionBox->SetIsReplicated(true);
	CustomCollisionBox->SetCanEverAffectNavigation(false);
	//CustomCollisionBox->OnComponentHit.AddDynamic(this, &AProceduralMeshTreeActor::OnCustomComponentHit);


	ProceduralMeshComponent->SetCanEverAffectNavigation(false);
	ProceduralMeshComponent->bUseComplexAsSimpleCollision = false;
	CutLeavesProceduralMeshComponent->SetCanEverAffectNavigation(false);
	CutLeavesProceduralMeshComponent->bUseComplexAsSimpleCollision = false;


	// Set other properties as needed
	Destroying = false;

	bReplicates = true;

	SmoothSync = CreateDefaultSubobject<USmoothSync>(TEXT("SmoothSyncPlugin"));
	SmoothSync->setSceneComponentToSync(CylinderMeshComponent);
	SmoothSync->setSceneComponentToSync(CustomCollisionBox);
	SmoothSync->setSceneComponentToSync(ProceduralMeshComponent);

	TreeSliceHeight = 50.0f;
	IsSetUp = 0;

	//SetReplicateMovement(true);
	//SetReplicatingMovement(true); //Turned off due to SmoothSync

}



bool AProceduralMeshTreeActor::GetStaticMeshVertexInfo(UStaticMeshComponent* SM, TArray<FVector>& VertexPositions, TArray<FColor>& VertexColors)
{
	VertexPositions.Empty();
	VertexColors.Empty();

	if (!SM)
	{
		return false;
	}
	if (!SM->IsValidLowLevel())
	{
		return false;
	}

	FPositionVertexBuffer* VertexBuffer = &SM->GetStaticMesh()->GetRenderData()->LODResources[0].VertexBuffers.PositionVertexBuffer;
	FColorVertexBuffer* ColorBuffer = &SM->GetStaticMesh()->GetRenderData()->LODResources[0].VertexBuffers.ColorVertexBuffer;

	if (VertexBuffer)
	{
		const int32 VertexCount = VertexBuffer->GetNumVertices();
		for (int32 Index = 0; Index < VertexCount; Index++)
		{
			// Converts the UStaticMesh vertex into world actor space including Translation, Rotation, and Scaling
			FVector3f Vertex = VertexBuffer->VertexPosition(Index);
			FVector Vertex3d = FVector(Vertex.X, Vertex.Y, Vertex.Z);
			FVector WSLocation = SM->GetOwner()->GetTransform().TransformVector(Vertex3d);
			//FVector WSLocation = SM->GetOwner()->GetTransform().TransformVector(VertexBuffer->VertexPosition(Index));
			VertexPositions.Add(WSLocation);

			const FColor Color = ColorBuffer->VertexColor(Index);
			VertexColors.Add(Color);
		}
	}

	return true;
}


void AProceduralMeshTreeActor::MultiCastSetupTree_Implementation(FTransform PointTransform, float SliceHeight, UMaterialInterface* CapMaterialParam, UStaticMesh* TreeCollisionMesh)
{
	int32 NumMats = StaticMeshComponent->GetNumMaterials();
	//NumMats = UKismetMathLibrary::Max(NumMats, 2);

	ProceduralMeshComponent->bAffectDistanceFieldLighting = true;
	for (int i = 0; i<1; i++) {
		TArray<FVector> ConvexVerts;
		TArray< int32 > Triangles;
		TArray< FVector > Normals;
		TArray< FVector2D > UVs;
		TArray< FProcMeshTangent > Tangents;
		TArray< FColor> Colors;
		TMap<FVector3f, FColor> VertexColorData;

		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(StaticMeshComponent->GetStaticMesh(), 0, i, ConvexVerts, Triangles, Normals, UVs, Tangents);

		ProceduralMeshComponent->CreateMeshSection(i, ConvexVerts, Triangles, Normals, UVs, Colors, Tangents, false);
		ProceduralMeshComponent->SetMaterial(i, StaticMeshComponent->GetStaticMesh()->GetMaterial(i));
	}

	if (NumMats > 1)
	{
		TArray<FVector> ConvexVerts;
		TArray< int32 > Triangles;
		TArray< FVector > Normals;
		TArray< FVector2D > UVs;
		TArray< FProcMeshTangent > Tangents;
		TArray< FColor> Colors;
		TMap<FVector3f, FColor> VertexColorData;
		UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(StaticMeshComponent->GetStaticMesh(), 0, 1, ConvexVerts, Triangles, Normals, UVs, Tangents);
		CutLeavesProceduralMeshComponent->CreateMeshSection(1, ConvexVerts, Triangles, Normals, UVs, Colors, Tangents, false);
		CutLeavesProceduralMeshComponent->SetMaterial(1, StaticMeshComponent->GetStaticMesh()->GetMaterial(1));
	}

	CylinderMeshComponent->SetStaticMesh(TreeCollisionMesh);

	TArray<FVector> ConvexVerts;
	TArray< int32 > Triangles;
	TArray< FVector > Normals;
	TArray< FVector2D > UVs;
	TArray< FProcMeshTangent > Tangents;
	TArray< FColor> Colors;
	UKismetProceduralMeshLibrary::GetSectionFromStaticMesh(CylinderMeshComponent->GetStaticMesh(), 0, 0, ConvexVerts, Triangles, Normals, UVs, Tangents);

	ProceduralMeshComponent->AddCollisionConvexMesh(ConvexVerts);
	CutLeavesProceduralMeshComponent->AddCollisionConvexMesh(ConvexVerts);
	
	CylinderMeshComponent->SetMobility(EComponentMobility::Movable);
	CylinderMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CylinderMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	//UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMeshComponent, 0, ProceduralMeshComponent, true);
	ProceduralMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ProceduralMeshComponent->SetVisibility(false);
	CutLeavesProceduralMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CutLeavesProceduralMeshComponent->SetVisibility(false);

	
	//UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMeshComponent, 0, ProceduralMeshComponent, true);
	ProceduralMeshComponent->SetScalarParameterValueOnMaterials(FName(TEXT("base_wind_speed")), 0.0f);
	ProceduralMeshComponent->SetScalarParameterValueOnMaterials(FName(TEXT("wind_speed")), 0.0f);
	ProceduralMeshComponent->SetScalarParameterValueOnMaterials(FName(TEXT("speed_base_wind_tree")), 0.0f);
	ProceduralMeshComponent->SetVisibility(false);
	CutLeavesProceduralMeshComponent->SetVisibility(false);
	FVector SlicePlanePosition = UKismetMathLibrary::TransformLocation(PointTransform, FVector{ 0,0,SliceHeight });
	FVector SlicePlaneNormal = UKismetMathLibrary::TransformDirection(PointTransform, FVector{ 0,0,-1 });

	UKismetProceduralMeshLibrary::SliceProceduralMesh(ProceduralMeshComponent, SlicePlanePosition, SlicePlaneNormal, true, CutOtherHalfTree, EProcMeshSliceCapOption::CreateNewSectionForCap, CapMaterialParam);

	float HighestZ = CutOtherHalfTree->GetLocalBounds().GetBox().Max.Z;
	float LowestZ = CutOtherHalfTree->GetLocalBounds().GetBox().Min.Z;
	float HighestZCylinder = CylinderMeshComponent->GetStaticMesh()->GetBounds().GetBox().Max.Z;
	float LowestZCylinder = CylinderMeshComponent->GetStaticMesh()->GetBounds().GetBox().Min.Z;

	FVector OldCOM = CylinderMeshComponent->BodyInstance.GetCOMPosition();
	FBodyInstance* BodyInst = CylinderMeshComponent->GetBodyInstance();
	FVector InertiaTensor = BodyInst->GetBodyInertiaTensor();

	//Update cylinder collision to better match height of tree
	CylinderMeshComponent->SetWorldScale3D(FVector(1, 1, 1.5*HighestZ/HighestZCylinder));
	
	//Update cylinder collision inertia to make it act like a shorter squat cylinder
	if (auto* BodyInstance = CylinderMeshComponent->GetBodyInstance(NAME_None, false))
	{
		FVector NewCOM = BodyInstance->GetCOMPosition();
		FVector COMNudge = OldCOM - NewCOM;
		BodyInstance->COMNudge = COMNudge;
		FPhysicsActorHandle PhysicsActorHandle = BodyInstance->ActorHandle;
		BodyInstance->UpdateMassProperties();
		FPhysicsCommand::ExecuteWrite(BodyInstance->GetPhysicsActorHandle(), [&](FPhysicsActorHandle& Actor)
			{
				FPhysicsInterface::SetMassSpaceInertiaTensor_AssumesLocked(Actor, InertiaTensor);
			});
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::SanitizeFloat(BodyInstance->GetBodyMass()));
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, BodyInstance->GetBodyInertiaTensor().ToString());
	}

	CylinderMeshComponent->SetRelativeLocation(FVector(0, 0, SliceHeight));

	CutOtherHalfTree->SetVisibility(false);
	//CutOtherHalfTree->AttachToComponent(CylinderMeshComponent, FAttachmentTransformRules::KeepWorldTransform);
	CutOtherHalfTree->SetCollisionProfileName("NoCollision");
	CutOtherHalfTree->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	HadHitEvent = false;
}



void AProceduralMeshTreeActor::MultiCastSpawnTree_Implementation(FTransform PointTransform, float SliceHeight)
{
	ProceduralMeshComponent->SetCollisionProfileName("BlockAll");
	ProceduralMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ProceduralMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	ProceduralMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
	ProceduralMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	ProceduralMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	CutOtherHalfTree->bReceivesDecals = false;

	CutLeavesProceduralMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CutLeavesProceduralMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TreeCutSound, GetActorLocation());

	////Make custom collision box at top of mesh
	//FBox MeshBounds = StaticMeshComponent->Bounds.GetBox();
	//float MeshBoxHeight = MeshBounds.Max.Z - MeshBounds.Min.Z;
	//FBox CollisionBounds = CylinderMeshComponent->GetStaticMesh()->GetBoundingBox();
	//float CollisionBoxHeight = CollisionBounds.Max.Z - CollisionBounds.Min.Z;
	//float MeshCollisionHeightRatio = MeshBoxHeight / CollisionBoxHeight;

	//float HighestZ = CutOtherHalfTree->GetLocalBounds().GetBox().Max.Z;
	//float LowestZ = CutOtherHalfTree->GetLocalBounds().GetBox().Min.Z;
	//FVector HighestPoint = { 0,0,HighestZ };
	//FVector MidPoint = { 0,0,(HighestZ + LowestZ) / 2 };
	//float BoxSize = FMath::Min(100 * MeshCollisionHeightRatio / 15, 100);
	//CustomCollisionBox->SetBoxExtent(FVector(BoxSize, BoxSize, BoxSize)); // Set the desired size
	//CustomCollisionBox->AttachToComponent(CutOtherHalfTree, FAttachmentTransformRules::KeepRelativeTransform);
	//CustomCollisionBox->SetRelativeLocation(HighestPoint);
	//CustomCollisionBox->SetCollisionProfileName("NoCollision");
	//CustomCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly); //oliCHANGED CutActor->CustomCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//CustomCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	//CustomCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Overlap);
	//CustomCollisionBox->SetVisibility(true);

	StaticMeshComponent->SetScalarParameterValueOnMaterials(FName(TEXT("base_wind_speed")), 0.0f);
	StaticMeshComponent->SetScalarParameterValueOnMaterials(FName(TEXT("speed_base_wind_tree")), 0.0f);
	StaticMeshComponent->SetScalarParameterValueOnMaterials(FName(TEXT("wind_speed")), 0.0f);

	ProceduralMeshComponent->SetVisibility(false); 
	CutLeavesProceduralMeshComponent->SetVisibility(false);
	CutOtherHalfTree->SetVisibility(false);

	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

}

void AProceduralMeshTreeActor::MultiCastShowProceduralMesh_Implementation()
{
	CylinderMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics); //new for client
	CylinderMeshComponent->SetSimulatePhysics(true);
	CylinderMeshComponent->SetEnableGravity(true);
	CutOtherHalfTree->AttachToComponent(CylinderMeshComponent, FAttachmentTransformRules::KeepWorldTransform);
	//FVector ForwardVector = GetActorForwardVector();
	ForwardVector.Normalize();
	FVector UpVector = { 0,0,-20000 };
	FVector MeshUpVector = StaticMeshComponent->GetUpVector();
	MeshUpVector.Normalize();

	CylinderMeshComponent->AddForce(ForwardVector * FallingForce, NAME_None, true);
	ForceNetUpdate();
	SetActorTickEnabled(true);

	CutLeavesProceduralMeshComponent->SetVisibility(true);
	CutLeavesProceduralMeshComponent->SetSimulatePhysics(true);
	CutLeavesProceduralMeshComponent->SetEnableGravity(true);
	
	//StaticMeshComponent->SetVisibility(false);

}

void AProceduralMeshTreeActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProceduralMeshTreeActor, StaticMeshComponent);
	DOREPLIFETIME(AProceduralMeshTreeActor, CylinderMeshComponent);
	DOREPLIFETIME(AProceduralMeshTreeActor, CustomCollisionBox);

	// ... Other replicated properties ...
}

void AProceduralMeshTreeActor::OnCustomCollisionOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HadHitEvent)
	{
		CylinderMeshComponent->SetPhysicsMaxAngularVelocityInDegrees(10);
		if (OverlappedComponent) {
			AActor* RootActor = OverlappedComponent->GetAttachmentRootActor();
			//UE_LOG(LogTemp, Warning, TEXT("Number of elements in ProcMeshArray: %d"), ProcMeshArray.Num());
			USceneComponent* AttachedParent = OverlappedComponent->GetAttachParent();
			if (AttachedParent) {
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Destroying component"));
				//UE_LOG(LogTemp, Warning, TEXT("Destroying Component"));
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, OtherComp->GetName());
				Destroying = true;
				HadHitEvent = true;
				MultiCastDestroyProceduralMeshComponent();
				//CutOtherHalfTree->DestroyComponent();
				//AttachedParent->DestroyComponent();
			}
		}
	}
}

void AProceduralMeshTreeActor::OnCustomComponentHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Component Hit"));
	if (!HadHitEvent)
	{
		HadHitEvent = true;
		Destroying = true;
		MultiCastDestroyProceduralMeshComponent();
	}
}

void AProceduralMeshTreeActor::MultiCastDestroyProceduralMeshComponent_Implementation()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), TreeFallSound, GetActorLocation());
	TreeDestruction();
	//MyDestroyComponent();
	FTimerHandle DeathTimerHandle;
	FTimerDelegate DeathTimerDelegate;
	DeathTimerDelegate.BindUFunction(this, FName("MyDestroyComponent"));
	GetWorldTimerManager().SetTimer(DeathTimerHandle, DeathTimerDelegate, 0.5f, false);
}

void AProceduralMeshTreeActor::MyDestroyComponent()
{
	SetActorTickEnabled(false);
	CutOtherHalfTree->DestroyComponent();
	CylinderMeshComponent->DestroyComponent();
	CutLeavesProceduralMeshComponent->DestroyComponent();
}


void AProceduralMeshTreeActor::InitializeAbilitySystem()
{
	Super::InitializeAbilitySystem();

	// Initialize default values for Health Set

	// This will fail to work if you try to do it before Super::InitializeAbilitySystem()
	// Can't move this to the constructor for that reason.

	HealthSet->InitMaxHealth(100.f);
	HealthSet->InitHealth(100.f);

	HealthComponent->OnHealthChanged.AddDynamic(this, &ThisClass::OnHealthChange);
}

void AProceduralMeshTreeActor::OnHealthChange(ULyraHealthComponent* HealthComponentHandle, float Health, float MaxHealth, AActor* OwningActor)
{
	OnTakeDamage(); //Implement blueprint for shaking tree
	if (HasAuthority())
	{
		if (!IsSetUp) MultiCastSetupTree(RootComponent->GetComponentToWorld(), TreeSliceHeight, CapMaterial, TreeCollisionMeshParam), IsSetUp = 1;
		if (HealthSet->GetHealth() <= 0)
		{
			SetLifeSpan(60);
			StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			CylinderMeshComponent->SetCollisionProfileName("BlockAll");
			CylinderMeshComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel14, ECR_Ignore);
			CylinderMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			CylinderMeshComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore); //new

			//UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(CutActorStaticMeshComponent, 0, CutActorProceduralMeshComponent, false);
			FTransform PointTransform = GetActorTransform();
			//FVector SlicePlanePosition = UKismetMathLibrary::TransformLocation(PointTransform, FVector{ 0,0,SliceHeight });

			//Make simple collision mesh from basic shape i.e. cylinder
			FBox MeshBounds = StaticMeshComponent->Bounds.GetBox();
			float MeshBoxHeight = MeshBounds.Max.Z - MeshBounds.Min.Z;
			FBox CollisionBounds = CylinderMeshComponent->GetStaticMesh()->GetBoundingBox();
			float CollisionBoxHeight = CollisionBounds.Max.Z - CollisionBounds.Min.Z;
			float MeshCollisionHeightRatio = MeshBoxHeight / CollisionBoxHeight;

			ForwardVector = GetActorLocation() - LastHitResult.Location;
			ForwardVector.Z = 0;

			float SliceHeightNew = UKismetMathLibrary::InverseTransformLocation(PointTransform, LastHitResult.Location).Z;
			//CutActorStaticMeshComponent->SetRelativeScale3D(FVector(1, 1, 1));
			MultiCastSpawnTree(PointTransform, SliceHeightNew);

			FTimerHandle TimerHandleHideTree;
			FTimerDelegate TimerDelegateHideTree;
			//TimerDelegateHideTree.BindUFunction(this, FName("DisplayCutDownTree"), this, MeshCollisionHeightRatio, GetActorForwardVector());
			//GetWorldTimerManager().SetTimer(TimerHandleHideTree, TimerDelegateHideTree, 0.5f, false);
			DisplayCutDownTree(MeshCollisionHeightRatio, SliceHeightNew);

			//MultiCastHideFoliage(World, CutActor->PointHandle, SystemTemplate);

		}
	}
}

float AProceduralMeshTreeActor::GetHealth()
{
	return HealthSet->GetHealth();
}

// Called when the game starts or when spawned
void AProceduralMeshTreeActor::BeginPlay()
{
	Super::BeginPlay();
	//CustomCollisionBox->OnComponentHit.AddDynamic(this, &AProceduralMeshTreeActor::OnCustomComponentHit);

}
// Called every frame
void AProceduralMeshTreeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector AngularVelocity;
	AngularVelocity = CylinderMeshComponent->GetPhysicsAngularVelocityInDegrees();
	FTransform Transform = UKismetMathLibrary::MakeTransform(CylinderMeshComponent->GetComponentLocation(), FRotator(0, 0, 0));
	UKismetMathLibrary::InverseTransformDirection(Transform, AngularVelocity);
	AngularVelocity.Z = 0.0f;
	UKismetMathLibrary::TransformDirection(Transform, AngularVelocity);
	CylinderMeshComponent->SetPhysicsAngularVelocityInDegrees(AngularVelocity);
}

void AProceduralMeshTreeActor::DisplayCutDownTree(float MeshCollisionHeightRatio, float SliceHeight)
{
	MultiCastShowProceduralMesh();

	FTimerHandle TimerHandleHideMesh;
	FTimerDelegate TimerDelegateHideMesh;
	TimerDelegateHideMesh.BindUFunction(this, FName("ChangeVisibleMesh"));
	//HideStaticMeshComponent(CutActor->StaticMeshComponent, CutActor, ForwardVector);
	GetWorldTimerManager().SetTimer(TimerHandleHideMesh, TimerDelegateHideMesh, 0.3f, false);

	//Make custom collision box at top of mesh
	float HighestZ = CutOtherHalfTree->GetLocalBounds().GetBox().Max.Z;
	float LowestZ = CutOtherHalfTree->GetLocalBounds().GetBox().Min.Z;
	float HighestZCylinder = CylinderMeshComponent->GetStaticMesh()->GetBounds().GetBox().Max.Z;
	float LowestZCylinder = CylinderMeshComponent->GetStaticMesh()->GetBounds().GetBox().Min.Z;
	float HighestXCylinder = CylinderMeshComponent->GetStaticMesh()->GetBounds().GetBox().Max.X;
	float LowestXCylinder = CylinderMeshComponent->GetStaticMesh()->GetBounds().GetBox().Min.X;

	FVector HighestPoint = { 0,0,HighestZ };
	FVector MidPoint = { 0,0,(HighestZ + LowestZ) / 2 };
	float BoxSize = FMath::Min(200 * MeshCollisionHeightRatio / 15, 200);
	//CustomCollisionBox->SetBoxExtent(FVector(BoxSize, BoxSize, BoxSize)); // Set the desired size @NEW CHANGE
	CustomCollisionBox->SetBoxExtent(FVector(1.05*(HighestXCylinder - LowestXCylinder) / 2, 1.05*(HighestXCylinder - LowestXCylinder) / 2, (HighestZ - LowestZ) / 2));

	
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, InertiaTensor.ToString());
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::SanitizeFloat(BodyInst->GetBodyMass()));


	CustomCollisionBox->AttachToComponent(CylinderMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
	//CustomCollisionBox->SetRelativeLocation(HighestPoint);
	CustomCollisionBox->SetRelativeLocation(FVector(0, 0, (LowestZ + HighestZ) / 2));
	CustomCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//CustomCollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	//CustomCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel9, ECollisionResponse::ECR_Overlap); //@NEW CHANGE
	CustomCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap); //@NEW CHANGE

	CustomCollisionBox->SetGenerateOverlapEvents(true);
	CustomCollisionBox->SetNotifyRigidBodyCollision(true);
	CustomCollisionBox->BodyInstance.bNotifyRigidBodyCollision = true;


	FTimerHandle TimerHandle;
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("DestroyProceduralMeshComponent"));
	GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, 10.0f, false);
}

void AProceduralMeshTreeActor::ChangeVisibleMesh()
{
	MultiCastHideStaticMeshComponent();
}

void AProceduralMeshTreeActor::MultiCastHideStaticMeshComponent_Implementation()
{
	ProceduralMeshComponent->SetVisibility(true);
	CutOtherHalfTree->SetVisibility(true);
	StaticMeshComponent->SetVisibility(false);
}

void AProceduralMeshTreeActor::DestroyProceduralMeshComponent()
{
	if (CutOtherHalfTree && !Destroying)
	{
		MultiCastDestroyProceduralMeshComponent();
	}
}

//void AProceduralMeshTreeActor::OnDeathStarted(AActor*)
//{
//
//}
//
//void AProceduralMeshTreeActor::OnDeathFinished(AActor*)
//{
//	if(CylinderMeshComponent)
//	{
//		Destroying = true;
//		CylinderMeshComponent->DestroyComponent();
//		MultiCastDestroyProceduralMeshComponent();
//	}
//}

