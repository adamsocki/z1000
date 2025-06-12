# Blender Level Exporter

Export Blender scenes as level data for the z1000 game engine.

## Setup

1. Open Blender
2. Go to **Scripting** workspace
3. Click **Open** and load `blender_level_exporter.py`
4. Click **Run Script** to register the exporter

## Usage

### Method 1: Export Menu
1. Go to **File > Export > Level (.json)**
2. Choose save location
3. Click **Export Level**

### Method 2: Run Script Directly  
1. Select objects you want to export
2. Run the script from text editor
3. Export dialog will appear

## Object Naming Convention

Name your objects with these prefixes to set entity types:

- **`wall_`** → Wall entities (e.g., `wall_brick`, `wall_concrete`)
- **`light_`** → Light entities (e.g., `light_lamp`, `light_candle`) 
- **`prop_`** → Prop entities (e.g., `prop_crate`, `prop_barrel`)
- **No prefix** → Default to Wall entities

## Examples

```
wall_brick_01     → Wall entity, mesh name: "brick_01"
light_torch_02    → Light entity, mesh name: "torch_02"  
prop_crate_wood   → Prop entity, mesh name: "crate_wood"
```

## Material Mapping

- Blender material names become `materialName` in JSON
- Objects without materials use `"defaultMaterial"`
- Make sure material names match your engine's material factory

## Coordinate System

- Blender Y-up is converted to Z-up for the engine
- Positions and rotations are automatically converted

## Loading in Engine

1. Export level from Blender
2. Place `.json` file in `levels/` directory
3. In game, press **L** key to load `levels/test_level.json`
4. Or modify the file path in `zayn.cpp` to load your exported level