#pragma once

#include "CoreMinimal.h"
#include "Dom/JsonObject.h"

/**
 * Handle material related commands for UnrealMCP
 */
class UNREALMCP_API FUnrealMCPMaterialCommands
{
public:
    FUnrealMCPMaterialCommands();

    /**
     * Handle material commands from JSON
     */
    static TSharedPtr<FJsonObject> HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params);

private:
    /**
     * Sets a material on a specific mesh component of an actor
     * Params: { "name": "ActorName", "material": "/Game/Materials/M_Wood", "index": 0 }
     */
    static TSharedPtr<FJsonObject> HandleSetActorMaterial(const TSharedPtr<FJsonObject>& Params);

    /**
     * Sets a scalar or vector parameter on a Dynamic Material Instance
     * Params: { "name": "ActorName", "param_name": "Color", "scalar": 1.5, "vector": [1,0,0] }
     */
    static TSharedPtr<FJsonObject> HandleSetMaterialParameter(const TSharedPtr<FJsonObject>& Params);
};
