// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RadarHUD.generated.h"

UCLASS()
class ARadarHUD : public AHUD
{
	GENERATED_BODY()

public:
	ARadarHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

protected:
	UPROPERTY(EditAnywhere, Category = Radar)
	FVector2D RadarStartLocation = FVector2D(0.9f, 0.2f);

	/** Radious of radar */
	UPROPERTY(EditAnywhere, Category = Radar)
	float RadarRadius = 100.f;

	UPROPERTY(EditAnywhere, Category = Radar)
	float DegreeStep = 0.25f;

	UPROPERTY(EditAnywhere, Category = Radar)
	float DrawPixelSize = 5.f;

private:
	 /** Return center of the radar position */
	FVector2D GetRadarCenterPosition();

	/** Draw radar */
	void DrawRadar();
};

