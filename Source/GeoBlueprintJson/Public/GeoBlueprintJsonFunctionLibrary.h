#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeoBlueprintJsonFunctionLibrary.generated.h"

// Forward declarations
class UEdGraphPin;
class UEdGraphNode;
class UEdGraph;
class UK2Node;
class UK2Node_CallFunction;
class UK2Node_Variable;
class UK2Node_Event;
class UK2Node_CustomEvent;
class UK2Node_IfThenElse;
class UK2Node_ExecutionSequence;
class UK2Node_Select;
class UBlueprint;

USTRUCT()
struct FBlueprintNodeData
{
    GENERATED_BODY()

    UPROPERTY()
    FString NodeName;

    UPROPERTY()
    FString NodeType;

    UPROPERTY()
    FVector2D Position;

    UPROPERTY()
    TArray<FString> InputPins;

    UPROPERTY()
    TArray<FString> OutputPins;

    UPROPERTY()
    TArray<FString> ConnectedNodes;

    UPROPERTY()
    TMap<FString, FString> NodeProperties;
};

UCLASS()
class GEOBLUEPRINTJSON_API UGeoBlueprintJsonFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Geo|JSON")
    static FString ConvertBlueprintToJson(UObject* Object);

    UFUNCTION(BlueprintCallable, Category = "Geo|JSON")
    static bool ConvertJsonToBlueprint(const FString& JsonString, UObject*& OutObject);

    UFUNCTION(BlueprintCallable, Category = "Geo|JSON")
    static FString GetBlueprintPropertyAsJson(UObject* Object, const FString& PropertyName);

    UFUNCTION(BlueprintCallable, Category = "Geo|JSON")
    static bool SetBlueprintPropertyFromJson(UObject* Object, const FString& PropertyName, const FString& JsonValue);

    UFUNCTION(BlueprintCallable, Category = "Geo|JSON")
    static FString ConvertBlueprintGraphToJson(UBlueprint* Blueprint);

    UFUNCTION(BlueprintCallable, Category = "Geo|JSON")
    static FString ConvertBlueprintNodeToJson(UEdGraphNode* Node);


    static FString ConvertBlueprintPinToJson(UEdGraphPin* Pin);

private:
    static TSharedPtr<FJsonObject> ConvertNodeToJsonObject(UEdGraphNode* Node);
    static TSharedPtr<FJsonObject> ConvertPinToJsonObject(UEdGraphPin* Pin);
    static TSharedPtr<FJsonObject> ConvertConnectionToJsonObject(UEdGraphPin* Pin);
    static TSharedPtr<FJsonObject> ConvertFunctionCallNodeToJsonObject(UK2Node_CallFunction* Node);
    static TSharedPtr<FJsonObject> ConvertVariableNodeToJsonObject(UK2Node_Variable* Node);
    static TSharedPtr<FJsonObject> ConvertEventNodeToJsonObject(UK2Node_Event* Node);
    static TSharedPtr<FJsonObject> ConvertCustomEventNodeToJsonObject(UK2Node_CustomEvent* Node);
    static TSharedPtr<FJsonObject> ConvertIfThenElseNodeToJsonObject(UK2Node_IfThenElse* Node);
    static TSharedPtr<FJsonObject> ConvertSequenceNodeToJsonObject(UK2Node_ExecutionSequence* Node);
    static TSharedPtr<FJsonObject> ConvertSelectNodeToJsonObject(UK2Node_Select* Node);
}; 