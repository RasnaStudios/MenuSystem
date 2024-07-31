// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

#include "MultiplayerSessionsSubsystem.h"

#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

UMultiplayerSessionsSubsystem::UMultiplayerSessionsSubsystem()
    : CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
    , FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete))
    , JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
    , DestroySessionCompleteDelegate(FOnDestroySessionCompleteDelegate::CreateUObject(this, &ThisClass::OnDestroySessionComplete))
    , StartSessionCompleteDelegate(FOnStartSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnStartSessionComplete))
{
    // Get the session interface from the online subsystem
    if (const IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get())
    {
        SessionInterface = Subsystem->GetSessionInterface();
    }
}

// Functions to handle session functionalities

void UMultiplayerSessionsSubsystem::CreateSession(int32 NumPublicConnections, FString MatchType)
{
    if (!SessionInterface.IsValid())
    {
        return;
    }
    // Destroy the existing session if it exists
    if (const auto ExistingSession = SessionInterface->GetNamedSession(NAME_GameSession); ExistingSession != nullptr)
    {
        SessionInterface->DestroySession(NAME_GameSession);
    }

    // Store the delegate handle, so we can remove it later from the delegate list
    SessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

    // We create the session settings
    LastSessionSettings = MakeShared<FOnlineSessionSettings>();

    // If there's a subsystem then the match is LAN, otherwise it's online
    LastSessionSettings->bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
    LastSessionSettings->NumPublicConnections = NumPublicConnections;    // Number of players that can join the session
    LastSessionSettings->bAllowJoinInProgress = true;     // Allow players to join the session even if it's already started
    LastSessionSettings->bAllowJoinViaPresence = true;    // Allow players to join the session via presence (friends list)
    LastSessionSettings->bShouldAdvertise = true;    // Advertise the session to the online subsystem so other players can find it
    LastSessionSettings->bUsesPresence = true;       // Use presence (friends list) to find the session
    LastSessionSettings->Set(
        FName("MatchType"), MatchType, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);    // Set the match type

    if (const ULocalPlayer* LocalPlayer = GetGameInstance()->GetFirstGamePlayer(); LocalPlayer != nullptr)
    {
        // Create the session and if it fails, remove the delegate handle
        if (!SessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *LastSessionSettings))
        {
            SessionInterface->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);
        }
    }
}

void UMultiplayerSessionsSubsystem::FindSessions(int32 MaxSearchResults)
{
}
void UMultiplayerSessionsSubsystem::JoinSession(const FOnlineSessionSearchResult& SearchResult)
{
}
void UMultiplayerSessionsSubsystem::DestroySession()
{
}
void UMultiplayerSessionsSubsystem::StartSession()
{
}

// Callbacks for delegates

void UMultiplayerSessionsSubsystem::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
void UMultiplayerSessionsSubsystem::OnFindSessionsComplete(bool bWasSuccessful)
{
}
void UMultiplayerSessionsSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
}
void UMultiplayerSessionsSubsystem::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
}
void UMultiplayerSessionsSubsystem::OnStartSessionComplete(FName SessionName, bool bWasSuccessful)
{
}
