#include "GeoBlueprintJsonFunctionLibrary.h"
#include "UObject/UnrealType.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "EdGraph/EdGraphPin.h"
#include "K2Node.h"
#include "K2Node_CallFunction.h"
#include "K2Node_Variable.h"
#include "K2Node_Event.h"
#include "K2Node_CustomEvent.h"
#include "K2Node_IfThenElse.h"
#include "K2Node_ExecutionSequence.h"
#include "K2Node_Select.h"
#include "K2Node_DynamicCast.h"
#include "K2Node_MacroInstance.h"
#include "K2Node_Timeline.h"
#include "K2Node_SpawnActor.h"
#include "K2Node_VariableGet.h"
#include "K2Node_VariableSet.h"
#include "K2Node_Knot.h"
#include "K2Node_Switch.h"
#include "K2Node_CommutativeAssociativeBinaryOperator.h"
#include "K2Node_MakeArray.h"
#include "K2Node_MakeStruct.h"
#include "K2Node_BreakStruct.h"
#include "K2Node_CallParentFunction.h"
#include "Blueprint/BlueprintSupport.h"
#include "Engine/Blueprint.h"

FString UGeoBlueprintJsonFunctionLibrary::ConvertBlueprintToJson(UObject* Object)
{
    if (!Object)
    {
        return TEXT("");
    }

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    
    // Iterate through all properties of the object
    for (TFieldIterator<FProperty> It(Object->GetClass()); It; ++It)
    {
        FProperty* Property = *It;
        FString PropertyName = Property->GetName();
        
        if (Property->IsA(FStrProperty::StaticClass()))
        {
            JsonObject->SetStringField(PropertyName, CastField<FStrProperty>(Property)->GetPropertyValue_InContainer(Object));
        }
        else if (Property->IsA(FIntProperty::StaticClass()))
        {
            JsonObject->SetNumberField(PropertyName, CastField<FIntProperty>(Property)->GetPropertyValue_InContainer(Object));
        }
        else if (Property->IsA(FFloatProperty::StaticClass()))
        {
            JsonObject->SetNumberField(PropertyName, CastField<FFloatProperty>(Property)->GetPropertyValue_InContainer(Object));
        }
        else if (Property->IsA(FBoolProperty::StaticClass()))
        {
            JsonObject->SetBoolField(PropertyName, CastField<FBoolProperty>(Property)->GetPropertyValue_InContainer(Object));
        }
        else if (Property->IsA(FObjectProperty::StaticClass()))
        {
            UObject* ObjValue = CastField<FObjectProperty>(Property)->GetObjectPropertyValue_InContainer(Object);
            if (ObjValue)
            {
                TSharedPtr<FJsonObject> NestedObject = MakeShared<FJsonObject>();
                // Recursively convert nested object
                FString NestedJson = ConvertBlueprintToJson(ObjValue);
                if (!NestedJson.IsEmpty())
                {
                    JsonObject->SetStringField(PropertyName, NestedJson);
                }
            }
        }
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    return OutputString;
}

bool UGeoBlueprintJsonFunctionLibrary::ConvertJsonToBlueprint(const FString& JsonString, UObject*& OutObject)
{
    if (JsonString.IsEmpty() || !OutObject)
    {
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        for (TFieldIterator<FProperty> It(OutObject->GetClass()); It; ++It)
        {
            FProperty* Property = *It;
            FString PropertyName = Property->GetName();
            
            const TSharedPtr<FJsonValue>* Value = JsonObject->Values.Find(PropertyName);
            if (Value)
            {
                if (Property->IsA(FStrProperty::StaticClass()))
                {
                    CastField<FStrProperty>(Property)->SetPropertyValue_InContainer(OutObject, (*Value)->AsString());
                }
                else if (Property->IsA(FIntProperty::StaticClass()))
                {
                    CastField<FIntProperty>(Property)->SetPropertyValue_InContainer(OutObject, (*Value)->AsNumber());
                }
                else if (Property->IsA(FFloatProperty::StaticClass()))
                {
                    CastField<FFloatProperty>(Property)->SetPropertyValue_InContainer(OutObject, (*Value)->AsNumber());
                }
                else if (Property->IsA(FBoolProperty::StaticClass()))
                {
                    CastField<FBoolProperty>(Property)->SetPropertyValue_InContainer(OutObject, (*Value)->AsBool());
                }
                else if (Property->IsA(FObjectProperty::StaticClass()))
                {
                    UObject* ObjValue = CastField<FObjectProperty>(Property)->GetObjectPropertyValue_InContainer(OutObject);
                    if (ObjValue)
                    {
                        FString NestedJson = (*Value)->AsString();
                        ConvertJsonToBlueprint(NestedJson, ObjValue);
                    }
                }
            }
        }
        return true;
    }

    return false;
}

FString UGeoBlueprintJsonFunctionLibrary::GetBlueprintPropertyAsJson(UObject* Object, const FString& PropertyName)
{
    if (!Object)
    {
        return TEXT("");
    }

    FProperty* Property = Object->GetClass()->FindPropertyByName(FName(*PropertyName));
    if (!Property)
    {
        return TEXT("");
    }

    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    
    if (Property->IsA(FStrProperty::StaticClass()))
    {
        JsonObject->SetStringField(PropertyName, CastField<FStrProperty>(Property)->GetPropertyValue_InContainer(Object));
    }
    else if (Property->IsA(FIntProperty::StaticClass()))
    {
        JsonObject->SetNumberField(PropertyName, CastField<FIntProperty>(Property)->GetPropertyValue_InContainer(Object));
    }
    else if (Property->IsA(FFloatProperty::StaticClass()))
    {
        JsonObject->SetNumberField(PropertyName, CastField<FFloatProperty>(Property)->GetPropertyValue_InContainer(Object));
    }
    else if (Property->IsA(FBoolProperty::StaticClass()))
    {
        JsonObject->SetBoolField(PropertyName, CastField<FBoolProperty>(Property)->GetPropertyValue_InContainer(Object));
    }
    else if (Property->IsA(FObjectProperty::StaticClass()))
    {
        UObject* ObjValue = CastField<FObjectProperty>(Property)->GetObjectPropertyValue_InContainer(Object);
        if (ObjValue)
        {
            FString NestedJson = ConvertBlueprintToJson(ObjValue);
            JsonObject->SetStringField(PropertyName, NestedJson);
        }
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    return OutputString;
}

bool UGeoBlueprintJsonFunctionLibrary::SetBlueprintPropertyFromJson(UObject* Object, const FString& PropertyName, const FString& JsonValue)
{
    if (!Object || JsonValue.IsEmpty())
    {
        return false;
    }

    FProperty* Property = Object->GetClass()->FindPropertyByName(FName(*PropertyName));
    if (!Property)
    {
        return false;
    }

    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonValue);

    if (FJsonSerializer::Deserialize(Reader, JsonObject))
    {
        const TSharedPtr<FJsonValue>* Value = JsonObject->Values.Find(PropertyName);
        if (Value)
        {
            if (Property->IsA(FStrProperty::StaticClass()))
            {
                CastField<FStrProperty>(Property)->SetPropertyValue_InContainer(Object, (*Value)->AsString());
                return true;
            }
            else if (Property->IsA(FIntProperty::StaticClass()))
            {
                CastField<FIntProperty>(Property)->SetPropertyValue_InContainer(Object, (*Value)->AsNumber());
                return true;
            }
            else if (Property->IsA(FFloatProperty::StaticClass()))
            {
                CastField<FFloatProperty>(Property)->SetPropertyValue_InContainer(Object, (*Value)->AsNumber());
                return true;
            }
            else if (Property->IsA(FBoolProperty::StaticClass()))
            {
                CastField<FBoolProperty>(Property)->SetPropertyValue_InContainer(Object, (*Value)->AsBool());
                return true;
            }
            else if (Property->IsA(FObjectProperty::StaticClass()))
            {
                UObject* ObjValue = CastField<FObjectProperty>(Property)->GetObjectPropertyValue_InContainer(Object);
                if (ObjValue)
                {
                    return ConvertJsonToBlueprint((*Value)->AsString(), ObjValue);
                }
            }
        }
    }

    return false;
}

FString UGeoBlueprintJsonFunctionLibrary::ConvertBlueprintGraphToJson(UBlueprint* Blueprint)
{
    if (!Blueprint)
    {
        return TEXT("");
    }

    TSharedPtr<FJsonObject> BlueprintJson = MakeShared<FJsonObject>();
    
    // Add basic Blueprint information
    BlueprintJson->SetStringField(TEXT("BlueprintName"), Blueprint->GetName());
    BlueprintJson->SetStringField(TEXT("BlueprintClass"), Blueprint->GetClass()->GetName());
    BlueprintJson->SetStringField(TEXT("ParentClass"), Blueprint->ParentClass ? Blueprint->ParentClass->GetName() : TEXT("None"));

    // Convert Event Graph
    TArray<TSharedPtr<FJsonValue>> EventGraphNodesArray;
    for (UEdGraph* Graph : Blueprint->UbergraphPages)
    {
        TSharedPtr<FJsonObject> GraphObject = MakeShared<FJsonObject>();
        GraphObject->SetStringField(TEXT("GraphName"), Graph->GetName());
        GraphObject->SetStringField(TEXT("GraphType"), TEXT("EventGraph"));
        
        TArray<TSharedPtr<FJsonValue>> GraphNodesArray;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            TSharedPtr<FJsonObject> NodeObject = ConvertNodeToJsonObject(Node);
            if (NodeObject.IsValid())
            {
                GraphNodesArray.Add(MakeShared<FJsonValueObject>(NodeObject));
            }
        }
        
        GraphObject->SetArrayField(TEXT("Nodes"), GraphNodesArray);
        EventGraphNodesArray.Add(MakeShared<FJsonValueObject>(GraphObject));
    }
    BlueprintJson->SetArrayField(TEXT("EventGraphs"), EventGraphNodesArray);

    // Convert Function Graphs
    TArray<TSharedPtr<FJsonValue>> FunctionGraphsArray;
    for (UEdGraph* Graph : Blueprint->FunctionGraphs)
    {
        TSharedPtr<FJsonObject> GraphObject = MakeShared<FJsonObject>();
        GraphObject->SetStringField(TEXT("GraphName"), Graph->GetName());
        GraphObject->SetStringField(TEXT("GraphType"), TEXT("FunctionGraph"));
        
        TArray<TSharedPtr<FJsonValue>> GraphNodesArray;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            TSharedPtr<FJsonObject> NodeObject = ConvertNodeToJsonObject(Node);
            if (NodeObject.IsValid())
            {
                GraphNodesArray.Add(MakeShared<FJsonValueObject>(NodeObject));
            }
        }
        
        GraphObject->SetArrayField(TEXT("Nodes"), GraphNodesArray);
        FunctionGraphsArray.Add(MakeShared<FJsonValueObject>(GraphObject));
    }
    BlueprintJson->SetArrayField(TEXT("FunctionGraphs"), FunctionGraphsArray);

    // Convert Macro Graphs
    TArray<TSharedPtr<FJsonValue>> MacroGraphsArray;
    for (UEdGraph* Graph : Blueprint->MacroGraphs)
    {
        TSharedPtr<FJsonObject> GraphObject = MakeShared<FJsonObject>();
        GraphObject->SetStringField(TEXT("GraphName"), Graph->GetName());
        GraphObject->SetStringField(TEXT("GraphType"), TEXT("MacroGraph"));
        
        TArray<TSharedPtr<FJsonValue>> GraphNodesArray;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            TSharedPtr<FJsonObject> NodeObject = ConvertNodeToJsonObject(Node);
            if (NodeObject.IsValid())
            {
                GraphNodesArray.Add(MakeShared<FJsonValueObject>(NodeObject));
            }
        }
        
        GraphObject->SetArrayField(TEXT("Nodes"), GraphNodesArray);
        MacroGraphsArray.Add(MakeShared<FJsonValueObject>(GraphObject));
    }
    BlueprintJson->SetArrayField(TEXT("MacroGraphs"), MacroGraphsArray);

    // Convert Delegate Graphs
    TArray<TSharedPtr<FJsonValue>> DelegateGraphsArray;
    for (UEdGraph* Graph : Blueprint->DelegateSignatureGraphs)
    {
        TSharedPtr<FJsonObject> GraphObject = MakeShared<FJsonObject>();
        GraphObject->SetStringField(TEXT("GraphName"), Graph->GetName());
        GraphObject->SetStringField(TEXT("GraphType"), TEXT("DelegateGraph"));
        
        TArray<TSharedPtr<FJsonValue>> GraphNodesArray;
        for (UEdGraphNode* Node : Graph->Nodes)
        {
            TSharedPtr<FJsonObject> NodeObject = ConvertNodeToJsonObject(Node);
            if (NodeObject.IsValid())
            {
                GraphNodesArray.Add(MakeShared<FJsonValueObject>(NodeObject));
            }
        }
        
        GraphObject->SetArrayField(TEXT("Nodes"), GraphNodesArray);
        DelegateGraphsArray.Add(MakeShared<FJsonValueObject>(GraphObject));
    }
    BlueprintJson->SetArrayField(TEXT("DelegateGraphs"), DelegateGraphsArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(BlueprintJson.ToSharedRef(), Writer);
    return OutputString;
}

FString UGeoBlueprintJsonFunctionLibrary::ConvertBlueprintNodeToJson(UEdGraphNode* Node)
{
    if (!Node)
    {
        return TEXT("");
    }

    TSharedPtr<FJsonObject> NodeObject = ConvertNodeToJsonObject(Node);
    if (!NodeObject.IsValid())
    {
        return TEXT("");
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(NodeObject.ToSharedRef(), Writer);
    return OutputString;
}

FString UGeoBlueprintJsonFunctionLibrary::ConvertBlueprintPinToJson(UEdGraphPin* Pin)
{
    if (!Pin)
    {
        return TEXT("");
    }

    TSharedPtr<FJsonObject> PinObject = ConvertPinToJsonObject(Pin);
    if (!PinObject.IsValid())
    {
        return TEXT("");
    }

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(PinObject.ToSharedRef(), Writer);
    return OutputString;
}

TSharedPtr<FJsonObject> UGeoBlueprintJsonFunctionLibrary::ConvertNodeToJsonObject(UEdGraphNode* Node)
{
    if (!Node)
    {
        return nullptr;
    }

    TSharedPtr<FJsonObject> NodeObject = MakeShared<FJsonObject>();
    
    // Basic node information
    NodeObject->SetStringField(TEXT("NodeName"), Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
    NodeObject->SetStringField(TEXT("NodeType"), Node->GetClass()->GetName());
    NodeObject->SetStringField(TEXT("NodeGuid"), Node->NodeGuid.ToString());
    NodeObject->SetNumberField(TEXT("NodeX"), Node->NodePosX);
    NodeObject->SetNumberField(TEXT("NodeY"), Node->NodePosY);
    NodeObject->SetStringField(TEXT("NodeComment"), Node->NodeComment);
    NodeObject->SetBoolField(TEXT("AdvancedPinDisplay"), static_cast<bool>(Node->AdvancedPinDisplay));
    NodeObject->SetBoolField(TEXT("EnabledState"), Node->IsNodeEnabled());

    // Convert input pins
    TArray<TSharedPtr<FJsonValue>> InputPinsArray;
    for (UEdGraphPin* Pin : Node->Pins)
    {
        if (Pin && Pin->Direction == EGPD_Input)
        {
            TSharedPtr<FJsonObject> PinObject = ConvertPinToJsonObject(Pin);
            if (PinObject.IsValid())
            {
                InputPinsArray.Add(MakeShared<FJsonValueObject>(PinObject));
            }
        }
    }
    NodeObject->SetArrayField(TEXT("InputPins"), InputPinsArray);

    // Convert output pins
    TArray<TSharedPtr<FJsonValue>> OutputPinsArray;
    for (UEdGraphPin* Pin : Node->Pins)
    {
        if (Pin && Pin->Direction == EGPD_Output)
        {
            TSharedPtr<FJsonObject> PinObject = ConvertPinToJsonObject(Pin);
            if (PinObject.IsValid())
            {
                OutputPinsArray.Add(MakeShared<FJsonValueObject>(PinObject));
            }
        }
    }
    NodeObject->SetArrayField(TEXT("OutputPins"), OutputPinsArray);

    // Add node-specific properties based on type
    if (Node->IsA<UK2Node_CallFunction>())
    {
        if (UK2Node_CallFunction* FunctionNode = Cast<UK2Node_CallFunction>(Node))
        {
            if (FunctionNode->FunctionReference.GetMemberParentClass())
            {
                NodeObject->SetStringField(TEXT("FunctionClass"), FunctionNode->FunctionReference.GetMemberParentClass()->GetName());
            }
            NodeObject->SetStringField(TEXT("FunctionName"), FunctionNode->FunctionReference.GetMemberName().ToString());
            
            if (UFunction* Function = FunctionNode->GetTargetFunction())
            {
                NodeObject->SetStringField(TEXT("FunctionSignature"), Function->GetName());
                NodeObject->SetBoolField(TEXT("IsPureFunc"), Function->HasAnyFunctionFlags(FUNC_BlueprintPure));
            }
        }
    }
    else if (Node->IsA<UK2Node_DynamicCast>())
    {
        if (UK2Node_DynamicCast* CastNode = Cast<UK2Node_DynamicCast>(Node))
        {
            if (UClass* TargetType = CastNode->TargetType)
            {
                NodeObject->SetStringField(TEXT("CastToType"), TargetType->GetName());
            }
            NodeObject->SetBoolField(TEXT("IsPureCast"), CastNode->IsNodePure());
        }
    }
    else if (Node->IsA<UK2Node_MacroInstance>())
    {
        if (UK2Node_MacroInstance* MacroNode = Cast<UK2Node_MacroInstance>(Node))
        {
            if (UBlueprint* MacroBlueprint = MacroNode->GetMacroGraph()->GetTypedOuter<UBlueprint>())
            {
                NodeObject->SetStringField(TEXT("MacroName"), MacroBlueprint->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_Timeline>())
    {
        if (UK2Node_Timeline* TimelineNode = Cast<UK2Node_Timeline>(Node))
        {
            NodeObject->SetStringField(TEXT("TimelineName"), TimelineNode->TimelineName.ToString());
        }
    }
    else if (Node->IsA<UK2Node_SpawnActor>())
    {
        if (UK2Node_SpawnActor* SpawnNode = Cast<UK2Node_SpawnActor>(Node))
        {
            // Use public API to get spawn class
            UClass* ClassToSpawn = nullptr;
            for (UEdGraphPin* Pin : SpawnNode->Pins)
            {
                if (Pin && Pin->PinName == TEXT("Class"))
                {
                    if (Pin->DefaultObject)
                    {
                        ClassToSpawn = Cast<UClass>(Pin->DefaultObject);
                        break;
                    }
                }
            }
            if (ClassToSpawn)
            {
                NodeObject->SetStringField(TEXT("ActorToSpawn"), ClassToSpawn->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_VariableGet>())
    {
        if (UK2Node_VariableGet* GetNode = Cast<UK2Node_VariableGet>(Node))
        {
            NodeObject->SetStringField(TEXT("VariableName"), GetNode->VariableReference.GetMemberName().ToString());
            if (GetNode->VariableReference.GetMemberParentClass())
            {
                NodeObject->SetStringField(TEXT("VariableClass"), GetNode->VariableReference.GetMemberParentClass()->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_VariableSet>())
    {
        if (UK2Node_VariableSet* SetNode = Cast<UK2Node_VariableSet>(Node))
        {
            NodeObject->SetStringField(TEXT("VariableName"), SetNode->VariableReference.GetMemberName().ToString());
            if (SetNode->VariableReference.GetMemberParentClass())
            {
                NodeObject->SetStringField(TEXT("VariableClass"), SetNode->VariableReference.GetMemberParentClass()->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_Knot>())
    {
        if (UK2Node_Knot* KnotNode = Cast<UK2Node_Knot>(Node))
        {
            // Knot nodes are simple pass-through nodes, just add their basic info
            NodeObject->SetBoolField(TEXT("IsKnot"), true);
        }
    }
    else if (Node->IsA<UK2Node_Switch>())
    {
        if (UK2Node_Switch* SwitchNode = Cast<UK2Node_Switch>(Node))
        {
            // Add switch-specific properties
            TArray<TSharedPtr<FJsonValue>> CasePinsArray;
            for (UEdGraphPin* Pin : SwitchNode->Pins)
            {
                if (Pin && Pin->Direction == EGPD_Output && !Pin->PinName.ToString().Contains(TEXT("Default")))
                {
                    CasePinsArray.Add(MakeShared<FJsonValueString>(Pin->PinName.ToString()));
                }
            }
            NodeObject->SetArrayField(TEXT("CasePins"), CasePinsArray);
        }
    }
    else if (Node->IsA<UK2Node_CommutativeAssociativeBinaryOperator>())
    {
        if (UK2Node_CommutativeAssociativeBinaryOperator* OpNode = Cast<UK2Node_CommutativeAssociativeBinaryOperator>(Node))
        {
            // Add operator-specific properties
            if (UFunction* OperatorFunction = OpNode->GetTargetFunction())
            {
                NodeObject->SetStringField(TEXT("OperatorFunction"), OperatorFunction->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_Event>())
    {
        if (UK2Node_Event* EventNode = Cast<UK2Node_Event>(Node))
        {
            if (EventNode->EventReference.GetMemberParentClass())
            {
                NodeObject->SetStringField(TEXT("EventClass"), EventNode->EventReference.GetMemberParentClass()->GetName());
            }
            NodeObject->SetStringField(TEXT("EventName"), EventNode->EventReference.GetMemberName().ToString());
            
            if (UFunction* Function = EventNode->FindEventSignatureFunction())
            {
                NodeObject->SetStringField(TEXT("EventSignature"), Function->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_CustomEvent>())
    {
        if (UK2Node_CustomEvent* CustomEventNode = Cast<UK2Node_CustomEvent>(Node))
        {
            NodeObject->SetStringField(TEXT("CustomEventName"), CustomEventNode->CustomFunctionName.ToString());
            
            if (UFunction* Function = CustomEventNode->FindEventSignatureFunction())
            {
                NodeObject->SetStringField(TEXT("CustomEventSignature"), Function->GetName());
            }
        }
    }
    else if (Node->IsA<UK2Node_IfThenElse>())
    {
        if (UK2Node_IfThenElse* IfThenElseNode = Cast<UK2Node_IfThenElse>(Node))
        {
            for (UEdGraphPin* Pin : IfThenElseNode->Pins)
            {
                if (Pin)
                {
                    if (Pin->Direction == EGPD_Input && Pin->PinName.ToString().Contains(TEXT("Condition")))
                    {
                        NodeObject->SetStringField(TEXT("ConditionPin"), Pin->PinName.ToString());
                    }
                    else if (Pin->Direction == EGPD_Output && Pin->PinName.ToString().Contains(TEXT("Then")))
                    {
                        NodeObject->SetStringField(TEXT("ThenPin"), Pin->PinName.ToString());
                    }
                    else if (Pin->Direction == EGPD_Output && Pin->PinName.ToString().Contains(TEXT("Else")))
                    {
                        NodeObject->SetStringField(TEXT("ElsePin"), Pin->PinName.ToString());
                    }
                }
            }
        }
    }
    else if (Node->IsA<UK2Node_Select>())
    {
        if (UK2Node_Select* SelectNode = Cast<UK2Node_Select>(Node))
        {
            int32 NumOptions = 0;
            for (UEdGraphPin* Pin : SelectNode->Pins)
            {
                if (Pin)
                {
                    if (Pin->Direction == EGPD_Input && Pin->PinName.ToString().Contains(TEXT("Index")))
                    {
                        NodeObject->SetStringField(TEXT("IndexPin"), Pin->PinName.ToString());
                    }
                    else if (Pin->Direction == EGPD_Output && Pin->PinName.ToString().Contains(TEXT("Selection")))
                    {
                        NodeObject->SetStringField(TEXT("SelectionPin"), Pin->PinName.ToString());
                    }
                    else if (Pin->Direction == EGPD_Input && Pin->PinName.ToString().StartsWith(TEXT("Option")))
                    {
                        NumOptions++;
                    }
                }
            }
            NodeObject->SetNumberField(TEXT("NumOptions"), NumOptions);
        }
    }
    else if (Node->IsA<UK2Node_MakeArray>())
    {
        if (UK2Node_MakeArray* ArrayNode = Cast<UK2Node_MakeArray>(Node))
        {
            NodeObject->SetNumberField(TEXT("NumElements"), ArrayNode->NumInputs);
            // Get array type from output pin
            for (UEdGraphPin* Pin : ArrayNode->Pins)
            {
                if (Pin && Pin->Direction == EGPD_Output)
                {
                    NodeObject->SetStringField(TEXT("ArrayType"), Pin->PinType.PinCategory.ToString());
                    break;
                }
            }
        }
    }
    else if (Node->IsA<UK2Node_MakeStruct>())
    {
        if (UK2Node_MakeStruct* StructNode = Cast<UK2Node_MakeStruct>(Node))
        {
            // Get struct type from output pin
            for (UEdGraphPin* Pin : StructNode->Pins)
            {
                if (Pin && Pin->Direction == EGPD_Output)
                {
                    if (UScriptStruct* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject.Get()))
                    {
                        NodeObject->SetStringField(TEXT("StructType"), Struct->GetName());
                        break;
                    }
                }
            }
        }
    }
    else if (Node->IsA<UK2Node_BreakStruct>())
    {
        if (UK2Node_BreakStruct* BreakNode = Cast<UK2Node_BreakStruct>(Node))
        {
            // Get struct type from input pin
            for (UEdGraphPin* Pin : BreakNode->Pins)
            {
                if (Pin && Pin->Direction == EGPD_Input)
                {
                    if (UScriptStruct* Struct = Cast<UScriptStruct>(Pin->PinType.PinSubCategoryObject.Get()))
                    {
                        NodeObject->SetStringField(TEXT("StructType"), Struct->GetName());
                        break;
                    }
                }
            }
        }
    }
    else if (Node->IsA<UK2Node_CallParentFunction>())
    {
        if (UK2Node_CallParentFunction* ParentNode = Cast<UK2Node_CallParentFunction>(Node))
        {
            if (UFunction* ParentFunction = ParentNode->GetTargetFunction())
            {
                NodeObject->SetStringField(TEXT("ParentFunction"), ParentFunction->GetName());
                if (ParentFunction->GetOwnerClass())
                {
                    NodeObject->SetStringField(TEXT("ParentClass"), ParentFunction->GetOwnerClass()->GetName());
                }
            }
        }
    }

    return NodeObject;
}

TSharedPtr<FJsonObject> UGeoBlueprintJsonFunctionLibrary::ConvertPinToJsonObject(UEdGraphPin* Pin)
{
    if (!Pin)
    {
        return nullptr;
    }

    TSharedPtr<FJsonObject> PinObject = MakeShared<FJsonObject>();
    
    PinObject->SetStringField(TEXT("PinName"), Pin->PinName.ToString());
    PinObject->SetStringField(TEXT("PinType"), Pin->PinType.PinCategory.ToString());
    PinObject->SetStringField(TEXT("Direction"), Pin->Direction == EGPD_Input ? TEXT("Input") : TEXT("Output"));

    // Convert connections
    TArray<TSharedPtr<FJsonValue>> ConnectionsArray;
    for (UEdGraphPin* ConnectedPin : Pin->LinkedTo)
    {
        TSharedPtr<FJsonObject> ConnectionObject = ConvertConnectionToJsonObject(ConnectedPin);
        if (ConnectionObject.IsValid())
        {
            ConnectionsArray.Add(MakeShared<FJsonValueObject>(ConnectionObject));
        }
    }
    PinObject->SetArrayField(TEXT("Connections"), ConnectionsArray);

    return PinObject;
}

TSharedPtr<FJsonObject> UGeoBlueprintJsonFunctionLibrary::ConvertConnectionToJsonObject(UEdGraphPin* Pin)
{
    if (!Pin || !Pin->GetOwningNode())
    {
        return nullptr;
    }

    TSharedPtr<FJsonObject> ConnectionObject = MakeShared<FJsonObject>();
    
    ConnectionObject->SetStringField(TEXT("NodeName"), Pin->GetOwningNode()->GetNodeTitle(ENodeTitleType::FullTitle).ToString());
    ConnectionObject->SetStringField(TEXT("PinName"), Pin->PinName.ToString());
    ConnectionObject->SetStringField(TEXT("PinType"), Pin->PinType.PinCategory.ToString());

    return ConnectionObject;
}

bool UGeoBlueprintJsonFunctionLibrary::ExportJsonToFile(const FString& JsonString, const FString& FilePath)
{
    if (JsonString.IsEmpty() || FilePath.IsEmpty())
    {
        return false;
    }

    // Ensure the directory exists
    FString Directory = FPaths::GetPath(FilePath);
    if (!FPaths::DirectoryExists(Directory))
    {
        IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
        PlatformFile.CreateDirectoryTree(*Directory);
    }

    // Write the JSON string to file
    return FFileHelper::SaveStringToFile(JsonString, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_None);
} 

FString UGeoBlueprintJsonFunctionLibrary::ExportAllAvailableNodesAsJson()
{
    TArray<TSharedPtr<FJsonValue>> AllNodesArray;
    
    // Get all classes that derive from UK2Node
    TArray<UClass*> K2NodeClasses;
    GetDerivedClasses(UK2Node::StaticClass(), K2NodeClasses);
    
    // Process K2Node classes
    for (UClass* NodeClass : K2NodeClasses)
    {
        if (!NodeClass || NodeClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
            continue;
            
        TSharedPtr<FJsonObject> NodeObject = MakeShareable(new FJsonObject);
        
        // Basic node information
        NodeObject->SetStringField(TEXT("node_type"), NodeClass->GetName());
        NodeObject->SetStringField(TEXT("display_name"), NodeClass->GetDisplayNameText().ToString());
        NodeObject->SetStringField(TEXT("category"), TEXT("K2Node"));
        
        // Get node tooltip
        FString Tooltip;
        if (NodeClass->HasMetaData(TEXT("Tooltip")))
        {
            Tooltip = NodeClass->GetMetaData(TEXT("Tooltip"));
        }
        NodeObject->SetStringField(TEXT("tooltip"), Tooltip);
        
        // Get node keywords for search
        FString Keywords;
        if (NodeClass->HasMetaData(TEXT("Keywords")))
        {
            Keywords = NodeClass->GetMetaData(TEXT("Keywords"));
        }
        NodeObject->SetStringField(TEXT("keywords"), Keywords);
        
        // Get node category
        FString Category;
        if (NodeClass->HasMetaData(TEXT("Category")))
        {
            Category = NodeClass->GetMetaData(TEXT("Category"));
        }
        NodeObject->SetStringField(TEXT("category_path"), Category);
        
        // Get node pins if it's a K2Node
        if (UK2Node* K2Node = Cast<UK2Node>(NodeClass->GetDefaultObject()))
        {
            TArray<TSharedPtr<FJsonValue>> PinsArray;
            for (UEdGraphPin* Pin : K2Node->Pins)
            {
                TSharedPtr<FJsonObject> PinObject = MakeShareable(new FJsonObject);
                PinObject->SetStringField(TEXT("name"), Pin->PinName.ToString());
                PinObject->SetStringField(TEXT("direction"), Pin->Direction == EGPD_Input ? TEXT("Input") : TEXT("Output"));
                PinObject->SetStringField(TEXT("type"), Pin->PinType.PinCategory.ToString());
                PinObject->SetStringField(TEXT("sub_type"), Pin->PinType.PinSubCategory.ToString());
                PinObject->SetBoolField(TEXT("is_array"), Pin->PinType.IsArray());
                PinObject->SetBoolField(TEXT("is_reference"), Pin->PinType.bIsReference);
                
                PinsArray.Add(MakeShareable(new FJsonValueObject(PinObject)));
            }
            NodeObject->SetArrayField(TEXT("pins"), PinsArray);
        }
        
        AllNodesArray.Add(MakeShareable(new FJsonValueObject(NodeObject)));
    }
    
    // Get all available Blueprint function libraries and their functions
    TArray<UClass*> FunctionLibraryClasses;
    GetDerivedClasses(UBlueprintFunctionLibrary::StaticClass(), FunctionLibraryClasses);
    
    for (UClass* LibraryClass : FunctionLibraryClasses)
    {
        if (!LibraryClass || LibraryClass->HasAnyClassFlags(CLASS_Abstract | CLASS_Deprecated | CLASS_NewerVersionExists))
            continue;
        
        // Get all UFUNCTIONs in the class
        for (TFieldIterator<UFunction> FuncIt(LibraryClass); FuncIt; ++FuncIt)
        {
            UFunction* Function = *FuncIt;
            if (!Function || Function->HasAnyFunctionFlags(FUNC_Private | FUNC_Protected))
                continue;
                
            // Check if the function is marked as BlueprintCallable or BlueprintPure
            if (!Function->HasAnyFunctionFlags(FUNC_BlueprintCallable | FUNC_BlueprintPure))
                continue;
                
            TSharedPtr<FJsonObject> NodeObject = MakeShareable(new FJsonObject);
            
            // Basic node information
            NodeObject->SetStringField(TEXT("node_type"), TEXT("K2Node_CallFunction"));
            NodeObject->SetStringField(TEXT("function_name"), Function->GetName());
            NodeObject->SetStringField(TEXT("class_name"), LibraryClass->GetName());
            NodeObject->SetStringField(TEXT("display_name"), Function->GetDisplayNameText().ToString());
            NodeObject->SetStringField(TEXT("category"), TEXT("BlueprintFunction"));
            
            // Get function tooltip
            FString Tooltip;
            if (Function->HasMetaData(TEXT("Tooltip")))
            {
                Tooltip = Function->GetMetaData(TEXT("Tooltip"));
            }
            NodeObject->SetStringField(TEXT("tooltip"), Tooltip);
            
            // Get function keywords for search
            FString Keywords;
            if (Function->HasMetaData(TEXT("Keywords")))
            {
                Keywords = Function->GetMetaData(TEXT("Keywords"));
            }
            NodeObject->SetStringField(TEXT("keywords"), Keywords);
            
            // Get function category
            FString Category;
            if (Function->HasMetaData(TEXT("Category")))
            {
                Category = Function->GetMetaData(TEXT("Category"));
            }
            NodeObject->SetStringField(TEXT("category_path"), Category);
            
            // Get function parameters
            TArray<TSharedPtr<FJsonValue>> ParametersArray;
            for (TFieldIterator<FProperty> PropIt(Function); PropIt; ++PropIt)
            {
                FProperty* Property = *PropIt;
                if (!Property || !Property->HasAnyPropertyFlags(CPF_Parm))
                    continue;
                    
                TSharedPtr<FJsonObject> ParamObject = MakeShareable(new FJsonObject);
                ParamObject->SetStringField(TEXT("name"), Property->GetName());
                ParamObject->SetStringField(TEXT("type"), Property->GetClass()->GetName());
                ParamObject->SetStringField(TEXT("display_name"), Property->GetDisplayNameText().ToString());
                ParamObject->SetStringField(TEXT("direction"), Property->HasAnyPropertyFlags(CPF_ReturnParm) ? TEXT("Return") : 
                    Property->HasAnyPropertyFlags(CPF_OutParm) ? TEXT("Output") : TEXT("Input"));
                
                ParametersArray.Add(MakeShareable(new FJsonValueObject(ParamObject)));
            }
            NodeObject->SetArrayField(TEXT("parameters"), ParametersArray);
            
            AllNodesArray.Add(MakeShareable(new FJsonValueObject(NodeObject)));
        }
    }
    
    // Convert to JSON string
    FString ResultJson;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&ResultJson);
    FJsonSerializer::Serialize(AllNodesArray, Writer);
    
    UE_LOG(LogTemp, Log, TEXT("Exported %d available nodes from the engine"), AllNodesArray.Num());
    return ResultJson;
}

