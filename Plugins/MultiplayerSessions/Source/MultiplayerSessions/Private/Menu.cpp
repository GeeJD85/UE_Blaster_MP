// Copyright Mintcakegames


#include "Menu.h"

#include "Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include "OnlineSessionSettings.h"


void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	SetIsFocusable(true);

	if (const TObjectPtr<UWorld> World = GetWorld())
	{
		if (const TObjectPtr<APlayerController> PlayerController = World->GetFirstPlayerController())
		{
			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(TakeWidget());
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(true);
		}
	}

	// Get and store the subsystem
	if (const TObjectPtr<UGameInstance> GameInstance = GetGameInstance())
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}

	// Bind callbacks
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (Button_Host)
	{
		Button_Host->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}

	if(Button_Join)
	{
		Button_Join->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	
	return true;
}

void UMenu::NativeDestruct()
{
	Super::NativeDestruct();

	MenuTearDown();
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Green,
				FString(TEXT("Session created successfully!"))
				);
		}

		if(TObjectPtr<UWorld> World = GetWorld())
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!"))
				);
		}
		Button_Host->SetIsEnabled(true);
	}
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr) return;
	
	// ADD THESE TO MENU TO CHOOSE A GAME - this iteration picks the first matching lobby type
	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if(SettingsValue == MatchType)
		{
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if(!bWasSuccessful || SessionResults.Num() == 0)
	{
		Button_Join->SetIsEnabled(true);
	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if(SessionInterface.IsValid())
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);

			if (const TObjectPtr<APlayerController> PlayerController = GetGameInstance()->GetFirstLocalPlayerController())
			{
				PlayerController->ClientTravel(Address, TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		Button_Join->SetIsEnabled(true);
	}
}


void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to start session!"))
				);
		}
		return;
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Green,
			FString(TEXT("Session started!"))
			);
	}
}

void UMenu::HostButtonClicked()
{
	Button_Host->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);		
	}
}

void UMenu::JoinButtonClicked()
{
	Button_Join->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(100000); // APPID 480 makes this high, lower for uniqueid
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	if(const TObjectPtr<UWorld> World = GetWorld())
	{
		if(const TObjectPtr<APlayerController> PlayerController = World->GetFirstPlayerController())
		{
			const FInputModeGameOnly InputModeData;
			PlayerController->SetInputMode(InputModeData);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
