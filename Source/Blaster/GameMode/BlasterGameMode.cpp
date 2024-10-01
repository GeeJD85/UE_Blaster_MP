// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Blaster/Character/BlasterCharacter.h"
#include "Blaster/PlayerController/BlasterPlayerController.h"
#include "Blaster/PlayerState/BlasterPlayerState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController,
	ABlasterPlayerController* AttackerController)
{
	TObjectPtr<ABlasterPlayerState> AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	TObjectPtr<ABlasterPlayerState> VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;

	if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
		// TODO: Function on PlayerState to display victim ID just killed 
	}
	if (VictimPlayerState && AttackerPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
		VictimPlayerState->ShowDefeatedByText(AttackerPlayerState->GetPlayerName());
	}
	
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset(); // Keep the PlayerController alive
		ElimmedCharacter->Destroy();
	}
	if (ElimmedController)
	{
		TObjectPtr<ABlasterPlayerState> ElimmedPlayerState = ElimmedCharacter ? Cast<ABlasterPlayerState>(ElimmedController->PlayerState) : nullptr;
		if(ElimmedPlayerState)
		{
			ElimmedPlayerState->ShowDefeatedByText(FString());
		}
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);		
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}