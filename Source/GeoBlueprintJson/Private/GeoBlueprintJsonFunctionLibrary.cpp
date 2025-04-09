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

    TSharedPtr<FJsonObject> GraphJson = MakeShared<FJsonObject>();
    TArray<TSharedPtr<FJsonValue>> NodesArray;

    // Convert all graphs in the Blueprint
    for (UEdGraph* Graph : Blueprint->UbergraphPages)
    {
        TSharedPtr<FJsonObject> GraphObject = MakeShared<FJsonObject>();
        GraphObject->SetStringField(TEXT("GraphName"), Graph->GetName());
        
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
        NodesArray.Add(MakeShared<FJsonValueObject>(GraphObject));
    }

    GraphJson->SetArrayField(TEXT("Graphs"), NodesArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(GraphJson.ToSharedRef(), Writer);
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
    NodeObject->SetNumberField(TEXT("NodeX"), Node->NodePosX);
    NodeObject->SetNumberField(TEXT("NodeY"), Node->NodePosY);

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
            }
        }
    }
    else if (Node->IsA<UK2Node_Variable>())
    {
        if (UK2Node_Variable* VariableNode = Cast<UK2Node_Variable>(Node))
        {
            if (VariableNode->VariableReference.GetMemberParentClass())
            {
                NodeObject->SetStringField(TEXT("VariableClass"), VariableNode->VariableReference.GetMemberParentClass()->GetName());
            }
            NodeObject->SetStringField(TEXT("VariableName"), VariableNode->VariableReference.GetMemberName().ToString());
            
            if (FProperty* Property = VariableNode->GetPropertyForVariable())
            {
                NodeObject->SetStringField(TEXT("VariableType"), Property->GetClass()->GetName());
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