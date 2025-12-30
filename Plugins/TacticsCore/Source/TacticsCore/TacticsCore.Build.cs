// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TacticsCore : ModuleRules
{
	public TacticsCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// Keep this module engine-agnostic as long as possible.
		// Only add CoreUObject/Engine when you have a concrete need.
		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
		});
	}
}
