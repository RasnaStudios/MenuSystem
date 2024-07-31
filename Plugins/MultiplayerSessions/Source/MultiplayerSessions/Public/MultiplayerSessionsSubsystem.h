// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "MultiplayerSessionsSubsystem.generated.h"

/**
 *
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMultiplayerSessionsSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    UMultiplayerSessionsSubsystem();
};
