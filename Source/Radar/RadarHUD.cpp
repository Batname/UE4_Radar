// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "RadarHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

ARadarHUD::ARadarHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}


void ARadarHUD::DrawHUD()
{
	Super::DrawHUD();

	// Draw very simple crosshair

	// find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));

	// draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );

	// -------------------- Radar logic --------------------
	DrawRadar();
	DrawPlayerInRadar();
	PerformRadarRaycast();
	DrawRaycastedActors();

	RadarActors.Empty();
}

FVector2D ARadarHUD::GetRadarCenterPosition()
{
	return (Canvas) ? FVector2D(Canvas->SizeX * RadarStartLocation.X, Canvas->SizeY * RadarStartLocation.Y) : FVector2D(0, 0);
}

void ARadarHUD::DrawRadar()
{
	FVector2D RadarCenter = GetRadarCenterPosition();

	for (float i = 0; i < 360; i+=DegreeStep)
	{
		float fixedX = FMath::Cos(i) * RadarRadius;
		float fixedY = FMath::Sin(i) * RadarRadius;

		DrawLine(RadarCenter.X, RadarCenter.Y, RadarCenter.X + fixedX, RadarCenter.Y + fixedY, FLinearColor::Gray, 1.f);
	}
}

void ARadarHUD::DrawPlayerInRadar()
{
	FVector2D RadarCenter = GetRadarCenterPosition();

	DrawRect(FLinearColor::Blue, RadarCenter.X, RadarCenter.Y, DrawPixelSize, DrawPixelSize);
}

void ARadarHUD::PerformRadarRaycast()
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Player)
	{
		TArray<FHitResult> HitResults;
		FVector EndLocation = Player->GetActorLocation();
		EndLocation.Z += SphereHeight;

		FCollisionShape CollisionShape;
		CollisionShape.ShapeType = ECollisionShape::Sphere;
		CollisionShape.SetSphere(SphereRadius);

		GetWorld()->SweepMultiByChannel(HitResults, Player->GetActorLocation(), EndLocation, FQuat::Identity, ECollisionChannel::ECC_WorldDynamic, CollisionShape);

		for (auto It : HitResults)
		{
			AActor* CurrentActor = It.GetActor();

			if (CurrentActor && CurrentActor->ActorHasTag("Radar"))
			{
				RadarActors.Add(CurrentActor);
			}
		}

	}
}


FVector2D ARadarHUD::ConvertWorldLocationToLocal(AActor* ActorToPlace)
{
	APawn* Player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (Player && ActorToPlace)
	{
		FVector ActorsLocal3dVector = Player->GetTransform().InverseTransformPosition(ActorToPlace->GetActorLocation());
		
		ActorsLocal3dVector = FRotator(0.f, -90.f, 0.f).RotateVector(ActorsLocal3dVector);

		ActorsLocal3dVector /= RadarDistanceScale;

		return FVector2D(ActorsLocal3dVector);
	}

	return FVector2D(0,0);
}

void ARadarHUD::DrawRaycastedActors()
{
	FVector2D RadarCenter = GetRadarCenterPosition();

	for (auto It : RadarActors)
	{
		FVector2D convertedLocation = ConvertWorldLocationToLocal(It);

		FVector tempVector = FVector(convertedLocation.X, convertedLocation.Y, 0.f);

		tempVector = tempVector.GetClampedToMaxSize2D(RadarRadius - DrawPixelSize);

		convertedLocation.X = tempVector.X;
		convertedLocation.Y = tempVector.Y;

		DrawRect(FLinearColor::Red, RadarCenter.X + convertedLocation.X, RadarCenter.Y + convertedLocation.Y, DrawPixelSize, DrawPixelSize);
	}
}