// Copyright Mintcakegames

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void Fire(const FVector& HitTarget) override;

protected:

	FVector TraceEndWithScatter(const FVector& TraceStart, const FVector& HitTarget);

private:
	UPROPERTY(EditAnywhere)
	float Damage = 20.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UParticleSystem> ImpactParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> BeamParticles;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class USoundCue> FireSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundCue> HitSound;

	/*
	 * Trace end with scatter
	 */
	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float DistanceToSphere = 800.f;

	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	float SphereRadius = 75.f;

	UPROPERTY(EditAnywhere, Category="Weapon Scatter")
	bool bUseScatter = false;
};
