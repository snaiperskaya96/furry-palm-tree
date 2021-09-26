// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Swarm : ModuleRules
{
	public Swarm(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "RenderCore", "Niagara", 
			"MeshMergeUtilities", "PhysicsCore"});
			//"MeshUtilities" });
	}
}
