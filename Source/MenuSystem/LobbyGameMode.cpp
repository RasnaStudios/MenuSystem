// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

#include "LobbyGameMode.h"

#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    // access the game state
    if (GameState)
    {
        const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(
                1, 600.f, FColor::Yellow, FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers));

        if (const APlayerState* PlayerState = NewPlayer->GetPlayerState<APlayerState>())
        {
            const FString PlayerName = PlayerState->GetPlayerName();
            if (GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 60.f, FColor::Yellow, TEXT("Player name: " + PlayerName));
        }
    }
}

void ALobbyGameMode::Logout(AController* Exiting)
{
    Super::Logout(Exiting);

    if (const APlayerState* PlayerState = Exiting->GetPlayerState<APlayerState>())
    {
        const int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
        if (GEngine)
            GEngine->AddOnScreenDebugMessage(1, 600.f, FColor::Yellow,
                FString::Printf(TEXT("Players in game: %d"), NumberOfPlayers - 1));    // TODO temporary hack
    }
}
