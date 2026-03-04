#include "Commands/UnrealMCPMaterialCommands.h"
#include "Commands/UnrealMCPCommonUtils.h"
#include "Components/MeshComponent.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"

FUnrealMCPMaterialCommands::FUnrealMCPMaterialCommands()
{
}

TSharedPtr<FJsonObject> FUnrealMCPMaterialCommands::HandleCommand(const FString& CommandType, const TSharedPtr<FJsonObject>& Params)
{
    if (CommandType == TEXT("set_actor_material"))
    {
        return HandleSetActorMaterial(Params);
    }
    else if (CommandType == TEXT("set_material_parameter"))
    {
        return HandleSetMaterialParameter(Params);
    }
    
    return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Unknown material command: %s"), *CommandType));
}

TSharedPtr<FJsonObject> FUnrealMCPMaterialCommands::HandleSetActorMaterial(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    if (!Params->TryGetStringField(TEXT("name"), ActorName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'name' parameter"));
    }

    FString MaterialPath;
    if (!Params->TryGetStringField(TEXT("material"), MaterialPath))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'material' parameter"));
    }

    int32 MaterialIndex = 0;
    Params->TryGetNumberField(TEXT("index"), MaterialIndex);

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    AActor* TargetActor = nullptr;
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Actor not found: %s"), *ActorName));
    }

    UMaterialInterface* Material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
    if (!Material)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(FString::Printf(TEXT("Material not found: %s"), *MaterialPath));
    }

    UMeshComponent* MeshComp = TargetActor->FindComponentByClass<UMeshComponent>();
    if (!MeshComp)
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Actor has no MeshComponent"));
    }

    MeshComp->SetMaterial(MaterialIndex, Material);

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetBoolField(TEXT("success"), true);
    return ResultObj;
}

TSharedPtr<FJsonObject> FUnrealMCPMaterialCommands::HandleSetMaterialParameter(const TSharedPtr<FJsonObject>& Params)
{
    FString ActorName;
    Params->TryGetStringField(TEXT("name"), ActorName);
    
    FString ParamName;
    if (!Params->TryGetStringField(TEXT("param_name"), ParamName))
    {
        return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Missing 'param_name'"));
    }

    TArray<AActor*> AllActors;
    UGameplayStatics::GetAllActorsOfClass(GWorld, AActor::StaticClass(), AllActors);
    
    AActor* TargetActor = nullptr;
    for (AActor* Actor : AllActors)
    {
        if (Actor && Actor->GetName() == ActorName)
        {
            TargetActor = Actor;
            break;
        }
    }

    if (!TargetActor) return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Actor not found"));

    UMeshComponent* MeshComp = TargetActor->FindComponentByClass<UMeshComponent>();
    if (!MeshComp) return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("No MeshComponent"));

    // Create DMI if it doesn't exist
    UMaterialInstanceDynamic* DMI = Cast<UMaterialInstanceDynamic>(MeshComp->GetMaterial(0));
    if (!DMI)
    {
        DMI = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
    }

    if (!DMI) return FUnrealMCPCommonUtils::CreateErrorResponse(TEXT("Failed to create DMI"));

    double ScalarValue;
    if (Params->TryGetNumberField(TEXT("scalar"), ScalarValue))
    {
        DMI->SetScalarParameterValue(FName(*ParamName), (float)ScalarValue);
    }

    const TArray<TSharedPtr<FJsonValue>>* VectorValue;
    if (Params->TryGetArrayField(TEXT("vector"), VectorValue) && VectorValue->Num() >= 3)
    {
        FLinearColor Color((*VectorValue)[0]->AsNumber(), (*VectorValue)[1]->AsNumber(), (*VectorValue)[2]->AsNumber());
        DMI->SetVectorParameterValue(FName(*ParamName), Color);
    }

    TSharedPtr<FJsonObject> ResultObj = MakeShared<FJsonObject>();
    ResultObj->SetBoolField(TEXT("success"), true);
    return ResultObj;
}
