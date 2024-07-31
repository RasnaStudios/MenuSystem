// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

using UnrealBuildTool;

public class MenuSystem : ModuleRules
{
    public MenuSystem(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "OnlineSubsystem", "OnlineSubsystemSteam"});
    }
}
