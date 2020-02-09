// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class SkillSystemEditor : ModuleRules
{
	public SkillSystemEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        PrivatePCHHeaderFile = "Editor/SkillSystemEditor/Public/SkillSystemEditor.h";
        PrivateIncludePaths.AddRange(
            new string[] {
                "Editor/SkillSystemEditor/Private",
                "Editor/SkillSystemEditor/Private/Details",
            }
		);

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
				"AssetRegistry",
				"AssetTools",
                "PropertyEditor",
				"ContentBrowser"
			}
		);

		PrivateDependencyModuleNames.AddRange(
			new string[] {
				"Core", 
				"CoreUObject", 
				"ApplicationCore",
				"Engine", 
                "RenderCore",
                "InputCore",
				"Slate",
				"SlateCore",
                "EditorStyle",
				"UnrealEd", 
                "AudioEditor",
				"MessageLog", 
				"GraphEditor",
                "Kismet",
				"KismetWidgets",
                "PropertyEditor",
				"AnimGraph",
				"BlueprintGraph",
                "AIGraph",
                "AIModule",
                "SkillSystem",
				"ClassViewer"
			}
		);

		PublicIncludePathModuleNames.Add("LevelEditor");

		DynamicallyLoadedModuleNames.AddRange(
            new string[] { 
                "WorkspaceMenuStructure",
				"AssetTools",
				"AssetRegistry",
				"ContentBrowser"
            }
		);
	}
}
