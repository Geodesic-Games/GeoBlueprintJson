# GeoBlueprintJson Plugin

A powerful Unreal Engine plugin that enables conversion of Blueprint graphs, nodes, and properties to JSON format. This plugin provides a comprehensive set of tools for exporting Blueprint structures and their relationships to JSON, making it ideal for Blueprint analysis, documentation, or external tool integration.

## Features

- Export entire Blueprint graphs to JSON format
- Convert individual Blueprint nodes to JSON
- Export Blueprint pin connections and relationships
- Support for multiple graph types:
  - Event Graphs
  - Function Graphs
  - Macro Graphs
  - Delegate Graphs
- Support for various node types:
  - Function Calls
  - Variables (Get/Set)
  - Events
  - Custom Events
  - Flow Control (If/Then/Else, Switch)
  - Math Operations
  - Casting
  - Array Operations
  - Struct Operations
  - Timeline
  - Spawn Actor
  - And more...

## Installation

1. Create a `Plugins` folder in your project if it doesn't exist
2. Copy the `GeoBlueprintJson` folder into your project's `Plugins` folder
3. Rebuild your project
4. Enable the plugin in Edit > Plugins > Project > Geo > GeoBlueprintJson

## Usage

### Blueprint Functions

The plugin provides several Blueprint-callable functions:

#### Convert Blueprint to JSON
```cpp
ConvertBlueprintGraphToJson(Blueprint)
```
Converts an entire Blueprint (including all graphs) to JSON format.

#### Convert Single Node to JSON
```cpp
ConvertBlueprintNodeToJson(Node)
```
Converts a single Blueprint node to JSON format.

#### Export JSON to File
```cpp
ExportJsonToFile(JsonString, FilePath)
```
Saves a JSON string to a file at the specified path.

### Example Usage in Blueprint

1. **Export Entire Blueprint:**
```
// Get the Blueprint asset
Get Blueprint Asset
-> Convert Blueprint Graph To Json
-> Export Json To File(JsonString, "C:/MyProject/Exports/MyBlueprint.json")
```

2. **Export Single Node:**
```
// Get a reference to a node
Get Node Reference
-> Convert Blueprint Node To Json
-> Export Json To File(JsonString, "C:/MyProject/Exports/Node.json")
```

### JSON Output Structure

The JSON output includes:

```json
{
    "BlueprintName": "MyBlueprint",
    "BlueprintClass": "Blueprint",
    "ParentClass": "Actor",
    "EventGraphs": [
        {
            "GraphName": "EventGraph",
            "GraphType": "EventGraph",
            "Nodes": [
                {
                    "NodeName": "Event BeginPlay",
                    "NodeType": "K2Node_Event",
                    "NodeGuid": "...",
                    "NodeX": 0,
                    "NodeY": 0,
                    "InputPins": [...],
                    "OutputPins": [...]
                }
            ]
        }
    ],
    "FunctionGraphs": [...],
    "MacroGraphs": [...],
    "DelegateGraphs": [...]
}
```

#### Specific Node Type Examples

- **Function Call Node:**
```json
{
    "NodeName": "Call Function",
    "NodeType": "K2Node_CallFunction",
    "FunctionName": "MyFunction",
    "FunctionClass": "MyClass",
    "NodeX": 100,
    "NodeY": 200,
    "InputPins": [...],
    "OutputPins": [...]
}
```

- **Variable Get Node:**
```json
{
    "NodeName": "Get MyVariable",
    "NodeType": "K2Node_VariableGet",
    "VariableName": "MyVariable",
    "VariableClass": "MyClass",
    "NodeX": 150,
    "NodeY": 250,
    "OutputPins": [...]
}
```

- **If-Then-Else Node:**
```json
{
    "NodeName": "Branch",
    "NodeType": "K2Node_IfThenElse",
    "ConditionPin": "Condition",
    "ThenPin": "Then",
    "ElsePin": "Else",
    "NodeX": 200,
    "NodeY": 300,
    "InputPins": [...],
    "OutputPins": [...]
}
```

## Configuration Options

The plugin can be configured through the following options:

- **Output Directory:** Specify the directory where JSON files will be saved. This can be set in the plugin settings or passed as a parameter to the `ExportJsonToFile` function.

- **File Naming Convention:** Customize the naming convention for exported JSON files. This can be done by modifying the file path parameter in the `ExportJsonToFile` function.

- **Include/Exclude Node Types:** Configure which node types to include or exclude in the JSON export. This can be adjusted in the plugin's source code to fit specific project needs.

## Requirements

- Unreal Engine 5.5 or later
- C++ project (for plugin compilation)

## Technical Details

- **Version:** 1.0.0
- **Supported Development Platforms:** Windows, Mac, Linux
- **Supported Target Build Platforms:** All platforms
- **Supported Engine Versions:** 5.5+

## License

This plugin is released under the MIT License. See the LICENSE file for details.

## Support

For bug reports and feature requests, please use the issue tracker on our repository.

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request 