// Copyright Epic Games, Inc. All Rights Reserved.

#include "MenuSystemCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputActionValue.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Online/OnlineSessionNames.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AMenuSystemCharacter

AMenuSystemCharacter::AMenuSystemCharacter()
    :    // These create and set the delegates to call when the session is created and found
    CreateSessionCompleteDelegate(FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete))
    , FindSessionsCompleteDelegate(FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionsComplete))
    , JoinSessionCompleteDelegate(FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete))
{
    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;               // Character moves in the direction of input...
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);    // ...at this rotation rate

    // Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
    // instead of recompiling to adjust them
    GetCharacterMovement()->JumpZVelocity = 700.f;
    GetCharacterMovement()->AirControl = 0.35f;
    GetCharacterMovement()->MaxWalkSpeed = 500.f;
    GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
    GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
    GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 400.0f;          // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true;    // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(
        CameraBoom, USpringArmComponent::SocketName);    // Attach the camera to the end of the boom and let the boom adjust to
                                                         // match the controller orientation
    FollowCamera->bUsePawnControlRotation = false;       // Camera does not rotate relative to arm

    // Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
    // are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

    IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
    if (OnlineSubsystem)
    {
        OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue,
                FString::Printf(TEXT("Found subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
        }
    }
}

void AMenuSystemCharacter::BeginPlay()
{
    // Call the base class
    Super::BeginPlay();
}

void AMenuSystemCharacter::CreateGameSession()
{
    // called when pressed 1 key
    if (!OnlineSessionInterface.IsValid())
    {
        return;
    }
    // NAME_GameSession is a predefined name for the session which is accessible globally
    auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);
    // we only want to create a session if one doesn't already exist
    if (ExistingSession != nullptr)
    {
        OnlineSessionInterface->DestroySession(NAME_GameSession);
    }

    // We need to set the delegate to call when the session is created
    OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

    // We create a new session settings object where MakeShareable is a helper function to create a shared pointer
    TSharedPtr<FOnlineSessionSettings> SessionSettings = MakeShareable(new FOnlineSessionSettings());
    // We set some session settings
    SessionSettings->bIsLANMatch = false;            // This is a LAN match
    SessionSettings->NumPublicConnections = 4;       // This is the maximum number of players that can join the session
    SessionSettings->bAllowJoinInProgress = true;    // This allows players to join the session even if it has already started
    SessionSettings->bAllowJoinViaPresence =
        true;    // This allows players to join the session via presence which is a steam feature to join friends in the same region
    SessionSettings->bShouldAdvertise =
        true;    // This allows steam to advertise the session so that other players can see it and join
    SessionSettings->bUsesPresence = true;             // This allows the session to use presence (steam)
    SessionSettings->bUseLobbiesIfAvailable = true;    // This allows the session to use lobbies if available, otherwise it crashes

    // We set the match type, so we can search for sessions with the same match type
    SessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    // We need a local player to get a unique net id
    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    // We now create a session with the unique net id, the name NAME_GameSession and the settings we just created
    OnlineSessionInterface->CreateSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *SessionSettings);
}

void AMenuSystemCharacter::JoinGameSession()
{
    // Find game sessions

    if (!OnlineSessionInterface.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("OnlineSessionInterface is not valid"));
        return;
    }

    // we set the delegate to call when the session search is complete
    OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

    // We search for sessions with the name NAME_GameSession
    SessionSearch = MakeShareable(new FOnlineSessionSearch());

    // We set some search settings
    SessionSearch->MaxSearchResults = 10000;    // This is the maximum number of sessions to find
    SessionSearch->bIsLanQuery = false;         // This is a LAN query
    SessionSearch->QuerySettings.Set(
        SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);    // Allows to search for sessions with steam presence

    // We need a local player to get a unique net id
    const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
    // We now search for sessions with the unique net id and the settings we just created
    OnlineSessionInterface->FindSessions(*LocalPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

// The delegate function to call when the session creation is complete to check that the session was created correctly
void AMenuSystemCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
    if (bWasSuccessful)
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, TEXT("Created session: " + SessionName.ToString()));
        }

        UWorld* World = GetWorld();
        if (World)
        {
            World->ServerTravel("/Game/Maps/Lobby?listen");
        }
    }
    else
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, TEXT("Failed to create session"));
        }
    }
}

// The delegate function to call when the session search is complete to check that any sessions were found
void AMenuSystemCharacter::OnFindSessionsComplete(bool bWasSuccessful)
{
    if (!OnlineSessionInterface.IsValid())
    {
        return;
    }
    for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
    {
        FString ID = SearchResult.GetSessionIdStr();
        FString User = SearchResult.Session.OwningUserName;
        FString MatchType = SearchResult.Session.SessionSettings.Settings.FindRef(FName("MatchType")).Data.ToString();

        FString NumPlayers = FString::FromInt(SearchResult.Session.SessionSettings.NumPublicConnections);
        FString MaxPlayers = FString::FromInt(SearchResult.Session.SessionSettings.NumPublicConnections);
        FString Ping = FString::FromInt(SearchResult.PingInMs);
        if (GEngine)
        {
            FString Result = FString::Printf(TEXT("ID: %s, User: %s, Match Type: %s, NumPlayers: %s, MaxPlayers: %s, Ping: %s"),
                *ID, *User, *MatchType, *NumPlayers, *MaxPlayers, *Ping);
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, *Result);
        }
        // We join the session if it is a FreeForAll match
        if (MatchType == "FreeForAll")
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, TEXT("Joining Match Type: " + MatchType));
            }

            // We set the delegate to call when the session join is complete
            OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);

            // We join the session with the unique net id and the session we found
            const ULocalPlayer* LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController();
            OnlineSessionInterface->JoinSession(*LocalPlayer->GetPreferredUniqueNetId(), NAME_GameSession, SearchResult);
        }
    }
}

void AMenuSystemCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if (!OnlineSessionInterface.IsValid())
    {
        return;
    }

    // We get the connect string to join the session from the session interface. This is an IP address and port
    FString ConnectString;
    if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, ConnectString))
    {
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, TEXT("Connect string: " + ConnectString));
        }
    }

    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        // We travel to the lobby map with the connect string
        PlayerController->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
    }
}

//////////////////////////////////////////////////////////////////////////
// Input

void AMenuSystemCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    // Add Input Mapping Context
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
                ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            Subsystem->AddMappingContext(DefaultMappingContext, 0);
        }
    }

    // Set up action bindings
    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // Jumping
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
        EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

        // Moving
        EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMenuSystemCharacter::Move);

        // Looking
        EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMenuSystemCharacter::Look);
    }
    else
    {
        UE_LOG(LogTemplateCharacter, Error,
            TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you "
                 "intend to use the legacy system, then you will need to update this C++ file."),
            *GetNameSafe(this));
    }
}

void AMenuSystemCharacter::Move(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D MovementVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // get forward vector
        const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

        // get right vector
        const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

        // add movement
        AddMovementInput(ForwardDirection, MovementVector.Y);
        AddMovementInput(RightDirection, MovementVector.X);
    }
}

void AMenuSystemCharacter::Look(const FInputActionValue& Value)
{
    // input is a Vector2D
    FVector2D LookAxisVector = Value.Get<FVector2D>();

    if (Controller != nullptr)
    {
        // add yaw and pitch input to controller
        AddControllerYawInput(LookAxisVector.X);
        AddControllerPitchInput(LookAxisVector.Y);
    }
}
