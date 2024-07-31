// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MenuSystemTarget : TargetRules
{
    public MenuSystemTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("MenuSystem");
    }
}
