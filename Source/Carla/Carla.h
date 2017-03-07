// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

// This file is included before any other file in every compile unit within the
// plugin.
#pragma once

#include "ModuleManager.h"

#include "NonCopyable.h"

DECLARE_LOG_CATEGORY_EXTERN(LogCarla, Log, All);

class FCarlaModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
