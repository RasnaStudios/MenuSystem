// Copyright (c) 2023-2024 Rasna Studios. All rights reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class MenuSystemEditorTarget : TargetRules
{
    public MenuSystemEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_4;
        ExtraModuleNames.Add("MenuSystem");
    }
}
