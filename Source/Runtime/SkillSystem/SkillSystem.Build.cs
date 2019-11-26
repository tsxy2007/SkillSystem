// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
    public class SkillSystem : ModuleRules
    {
        public SkillSystem(ReadOnlyTargetRules Target) : base(Target)
        {
            PrivatePCHHeaderFile = "Runtime/SkillSystem/Public/SkillSystem.h";
            PublicIncludePaths.AddRange(
                new string[] {
                    "Runtime/SkillSystem/Public",
                      "Runtime/SkillSystem/Public/Manage",
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                    "Runtime/SkillSystem/Private",
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "GameplayTags",
                    "GameplayTasks",
                    "NavigationSystem",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "RHI",
                    "RenderCore",
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[] {
					// ... add any modules that your module loads dynamically here ...
				}
                );

            if (Target.bBuildEditor == true)
            {
                PrivateDependencyModuleNames.Add("UnrealEd");
                
            }

            if (Target.bCompileRecast)
            {
                PrivateDependencyModuleNames.Add("Navmesh");
                PublicDefinitions.Add("WITH_RECAST=1");
            }
            else
            {
                // Because we test WITH_RECAST in public Engine header files, we need to make sure that modules
                // that import us also have this definition set appropriately.  Recast is a private dependency
                // module, so it's definitions won't propagate to modules that import Engine.
                PublicDefinitions.Add("WITH_RECAST=0");
            }

            if (Target.bBuildDeveloperTools || (Target.Configuration != UnrealTargetConfiguration.Shipping && Target.Configuration != UnrealTargetConfiguration.Test))
            {
                PrivateDependencyModuleNames.Add("GameplayDebugger");
                PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=1");
            }
            else
            {
                PublicDefinitions.Add("WITH_GAMEPLAY_DEBUGGER=0");
            }
        }
    }
}
