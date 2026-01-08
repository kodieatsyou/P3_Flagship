// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class TacticsCore : ModuleRules
{
	public TacticsCore(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});

		PrivateDependencyModuleNames.AddRange(new string[]
		{
			"Core"
		});
	}
}
