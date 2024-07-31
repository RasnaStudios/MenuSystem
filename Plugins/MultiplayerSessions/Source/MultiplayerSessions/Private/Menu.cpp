// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

#include "Menu.h"

void UMenu::SetupMenu()
{
    AddToViewport();
    SetVisibility(ESlateVisibility::Visible);
    bIsFocusable = true;

    if (const UWorld* World = GetWorld())
    {
        if (APlayerController* PlayerController = World->GetFirstPlayerController())
        {
            // Set the input mode to UI
            FInputModeUIOnly InputModeData;
            InputModeData.SetWidgetToFocus(TakeWidget());
            InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

            // We set the input mode to UI only and show the mouse cursor
            PlayerController->SetInputMode(InputModeData);
            PlayerController->bShowMouseCursor = true;
        }
    }
}
