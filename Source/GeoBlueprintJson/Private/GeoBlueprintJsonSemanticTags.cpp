#include "GeoBlueprintJsonSemanticTags.h"

TArray<EBlueprintJsonSemanticTag> UGeoBlueprintJsonSemanticTags::GetNodeSemanticTags(const FString& NodeType, const TMap<FString, FString>& NodeProperties)
{
    TArray<EBlueprintJsonSemanticTag> Tags;

    // Add node type tags
    if (NodeType.Contains(TEXT("Event")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::EventNode);
    }
    else if (NodeType.Contains(TEXT("Function")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::FunctionNode);
    }
    else if (NodeType.Contains(TEXT("Variable")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::VariableNode);
    }
    else if (NodeType.Contains(TEXT("IfThenElse")) || NodeType.Contains(TEXT("Switch")) || NodeType.Contains(TEXT("Sequence")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::FlowControlNode);
    }
    else if (NodeType.Contains(TEXT("MakeArray")) || NodeType.Contains(TEXT("MakeStruct")) || NodeType.Contains(TEXT("BreakStruct")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::DataOperationNode);
    }
    else if (NodeType.Contains(TEXT("Timeline")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::TimelineNode);
    }
    else if (NodeType.Contains(TEXT("Spawn")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::SpawnNode);
    }
    else if (NodeType.Contains(TEXT("Cast")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::CastNode);
    }
    else if (NodeType.Contains(TEXT("Macro")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::MacroNode);
    }
    else if (NodeType.Contains(TEXT("CustomEvent")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::CustomEventNode);
    }

    // Add property-based tags
    if (const FString* IsPure = NodeProperties.Find(TEXT("IsPureFunc")))
    {
        if (*IsPure == TEXT("true"))
        {
            Tags.Add(EBlueprintJsonSemanticTag::PureFunction);
        }
    }

    if (const FString* HasComment = NodeProperties.Find(TEXT("NodeComment")))
    {
        if (!HasComment->IsEmpty())
        {
            Tags.Add(EBlueprintJsonSemanticTag::HasComment);
        }
    }

    if (const FString* AdvancedPins = NodeProperties.Find(TEXT("AdvancedPinDisplay")))
    {
        if (*AdvancedPins == TEXT("true"))
        {
            Tags.Add(EBlueprintJsonSemanticTag::HasAdvancedPins);
        }
    }

    if (const FString* IsEnabled = NodeProperties.Find(TEXT("EnabledState")))
    {
        if (*IsEnabled == TEXT("true"))
        {
            Tags.Add(EBlueprintJsonSemanticTag::IsEnabled);
        }
        else
        {
            Tags.Add(EBlueprintJsonSemanticTag::IsDisabled);
        }
    }

    return Tags;
}

TArray<EBlueprintJsonSemanticTag> UGeoBlueprintJsonSemanticTags::GetPinSemanticTags(const FString& PinType, const TMap<FString, FString>& PinProperties)
{
    TArray<EBlueprintJsonSemanticTag> Tags;

    // Add pin type tags
    if (const FString* Direction = PinProperties.Find(TEXT("Direction")))
    {
        if (*Direction == TEXT("Input"))
        {
            Tags.Add(EBlueprintJsonSemanticTag::InputPin);
        }
        else if (*Direction == TEXT("Output"))
        {
            Tags.Add(EBlueprintJsonSemanticTag::OutputPin);
        }
    }

    // Add pin category tags
    if (PinType.Contains(TEXT("exec")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::ExecutionPin);
    }
    else if (PinType.Contains(TEXT("delegate")))
    {
        Tags.Add(EBlueprintJsonSemanticTag::DelegatePin);
    }
    else
    {
        Tags.Add(EBlueprintJsonSemanticTag::DataPin);
    }

    return Tags;
}

TArray<EBlueprintJsonSemanticTag> UGeoBlueprintJsonSemanticTags::GetGraphSemanticTags(const FString& GraphType, const TArray<FString>& NodeTypes)
{
    TArray<EBlueprintJsonSemanticTag> Tags;

    // Add graph type tag
    if (GraphType == TEXT("EventGraph"))
    {
        Tags.Add(EBlueprintJsonSemanticTag::EventGraph);
    }
    else if (GraphType == TEXT("FunctionGraph"))
    {
        Tags.Add(EBlueprintJsonSemanticTag::FunctionGraph);
    }
    else if (GraphType == TEXT("MacroGraph"))
    {
        Tags.Add(EBlueprintJsonSemanticTag::MacroGraph);
    }
    else if (GraphType == TEXT("DelegateGraph"))
    {
        Tags.Add(EBlueprintJsonSemanticTag::DelegateGraph);
    }

    // Add content-based tags
    for (const FString& NodeType : NodeTypes)
    {
        if (NodeType.Contains(TEXT("Variable")))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasVariables);
        }
        else if (NodeType.Contains(TEXT("Function")))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasFunctions);
        }
        else if (NodeType.Contains(TEXT("Macro")))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasMacros);
        }
        else if (NodeType.Contains(TEXT("Delegate")))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasDelegates);
        }
        else if (NodeType.Contains(TEXT("Timeline")))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasTimelines);
        }
    }

    return Tags;
}

TArray<EBlueprintJsonSemanticTag> UGeoBlueprintJsonSemanticTags::GetBlueprintSemanticTags(const TMap<FString, FString>& BlueprintProperties, const TArray<FString>& GraphTypes)
{
    TArray<EBlueprintJsonSemanticTag> Tags;

    // Add property-based tags
    if (const FString* ParentClass = BlueprintProperties.Find(TEXT("ParentClass")))
    {
        if (*ParentClass != TEXT("None"))
        {
            Tags.Add(EBlueprintJsonSemanticTag::HasParentClass);
        }
    }

    if (const FString* Interfaces = BlueprintProperties.Find(TEXT("Interfaces")))
    {
        if (!Interfaces->IsEmpty())
        {
            Tags.Add(EBlueprintJsonSemanticTag::HasInterfaces);
        }
    }

    if (const FString* Components = BlueprintProperties.Find(TEXT("Components")))
    {
        if (!Components->IsEmpty())
        {
            Tags.Add(EBlueprintJsonSemanticTag::HasComponents);
        }
    }

    // Add graph-based tags
    for (const FString& GraphType : GraphTypes)
    {
        if (GraphType == TEXT("EventGraph"))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasConstructionScript);
        }
        else if (GraphType == TEXT("FunctionGraph"))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasFunctions);
        }
        else if (GraphType == TEXT("MacroGraph"))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasMacros);
        }
        else if (GraphType == TEXT("DelegateGraph"))
        {
            Tags.AddUnique(EBlueprintJsonSemanticTag::HasDelegates);
        }
    }

    return Tags;
} 