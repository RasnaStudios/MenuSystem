// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "Menu.generated.h"

class UMultiplayerSessionsSubsystem;
class UButton;

/**
 * UMenu class is a user interface widget that provides functionality for hosting and joining multiplayer sessions.
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual void NativeDestruct() override;

    /**
     * Sets up the menu. This function is callable from Blueprints.
     */
    UFUNCTION(BlueprintCallable)
    void MenuSetup(const int32 NumPublicConnections, FString MatchType);

protected:
    /**
     * Initializes the widget. This function is called when the widget is constructed.
     * @return true if the initialization is successful, false otherwise.
     */
    virtual bool Initialize() override;

private:
    // These buttons are binded in the blueprint and the names must match the names in the blueprint

    /** Button to host a multiplayer session. */
    UPROPERTY(meta = (BindWidget))
    UButton* HostButton;

    /** Button to join a multiplayer session. */
    UPROPERTY(meta = (BindWidget))
    UButton* JoinButton;

    // These functions must be UFUNCTION() to be binded in the blueprint

    // Function called when the HostButton is clicked.
    UFUNCTION()
    void HostButtonClicked();

    // Function called when the JoinButton is clicked.
    UFUNCTION()
    void JoinButtonClicked();

    void MenuTearDown();

    /** Subsystem for handling multiplayer sessions. */
    UPROPERTY()
    UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;

    int32 _NumPublicConnections = 4;
    FString _MatchType = "FreeForAll";
};
