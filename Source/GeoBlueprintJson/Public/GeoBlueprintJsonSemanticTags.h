#pragma once

#include "CoreMinimal.h"
#include "GeoBlueprintJsonSemanticTags.generated.h"

/**
 * Semantic tags for categorizing and identifying elements in Blueprint JSON exports
 */
UENUM(BlueprintType)
enum class EBlueprintJsonSemanticTag : uint8
{
    // Graph Types
    EventGraph UMETA(DisplayName = "Event Graph"),
    FunctionGraph UMETA(DisplayName = "Function Graph"),
    MacroGraph UMETA(DisplayName = "Macro Graph"),
    DelegateGraph UMETA(DisplayName = "Delegate Graph"),

    // Node Categories
    EventNode UMETA(DisplayName = "Event Node"),
    FunctionNode UMETA(DisplayName = "Function Node"),
    VariableNode UMETA(DisplayName = "Variable Node"),
    FlowControlNode UMETA(DisplayName = "Flow Control Node"),
    DataOperationNode UMETA(DisplayName = "Data Operation Node"),
    TimelineNode UMETA(DisplayName = "Timeline Node"),
    SpawnNode UMETA(DisplayName = "Spawn Node"),
    CastNode UMETA(DisplayName = "Cast Node"),
    MacroNode UMETA(DisplayName = "Macro Node"),
    CustomEventNode UMETA(DisplayName = "Custom Event Node"),

    // Pin Types
    InputPin UMETA(DisplayName = "Input Pin"),
    OutputPin UMETA(DisplayName = "Output Pin"),
    ExecutionPin UMETA(DisplayName = "Execution Pin"),
    DataPin UMETA(DisplayName = "Data Pin"),
    DelegatePin UMETA(DisplayName = "Delegate Pin"),

    // Node Properties
    PureFunction UMETA(DisplayName = "Pure Function"),
    LatentFunction UMETA(DisplayName = "Latent Function"),
    HasComment UMETA(DisplayName = "Has Comment"),
    HasAdvancedPins UMETA(DisplayName = "Has Advanced Pins"),
    IsEnabled UMETA(DisplayName = "Is Enabled"),
    IsDisabled UMETA(DisplayName = "Is Disabled"),

    // Connection Types
    ExecutionFlow UMETA(DisplayName = "Execution Flow"),
    DataFlow UMETA(DisplayName = "Data Flow"),
    DelegateBinding UMETA(DisplayName = "Delegate Binding"),

    // Graph Properties
    HasVariables UMETA(DisplayName = "Has Variables"),
    HasFunctions UMETA(DisplayName = "Has Functions"),
    HasMacros UMETA(DisplayName = "Has Macros"),
    HasDelegates UMETA(DisplayName = "Has Delegates"),
    HasTimelines UMETA(DisplayName = "Has Timelines"),

    // Blueprint Properties
    HasParentClass UMETA(DisplayName = "Has Parent Class"),
    HasInterfaces UMETA(DisplayName = "Has Interfaces"),
    HasComponents UMETA(DisplayName = "Has Components"),
    HasConstructionScript UMETA(DisplayName = "Has Construction Script"),
    HasUserDefinedStructs UMETA(DisplayName = "Has User Defined Structs"),
    HasUserDefinedEnums UMETA(DisplayName = "Has User Defined Enums")
};

/**
 * Helper functions for working with semantic tags
 */
UCLASS()
class UGeoBlueprintJsonSemanticTags : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Get semantic tags for a node based on its type and properties
     */
    UFUNCTION(BlueprintCallable, Category = "Blueprint JSON")
    static TArray<EBlueprintJsonSemanticTag> GetNodeSemanticTags(const FString& NodeType, const TMap<FString, FString>& NodeProperties);

    /**
     * Get semantic tags for a pin based on its type and properties
     */
    UFUNCTION(BlueprintCallable, Category = "Blueprint JSON")
    static TArray<EBlueprintJsonSemanticTag> GetPinSemanticTags(const FString& PinType, const TMap<FString, FString>& PinProperties);

    /**
     * Get semantic tags for a graph based on its type and contents
     */
    UFUNCTION(BlueprintCallable, Category = "Blueprint JSON")
    static TArray<EBlueprintJsonSemanticTag> GetGraphSemanticTags(const FString& GraphType, const TArray<FString>& NodeTypes);

    /**
     * Get semantic tags for a blueprint based on its properties and contents
     */
    UFUNCTION(BlueprintCallable, Category = "Blueprint JSON")
    static TArray<EBlueprintJsonSemanticTag> GetBlueprintSemanticTags(const TMap<FString, FString>& BlueprintProperties, const TArray<FString>& GraphTypes);
}; 