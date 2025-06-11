import bpy
import json
import os
from mathutils import Vector

# Blender to Z1000 Engine Export Script
# Usage: 
# 1. Create cubes in Blender for walls, floors, etc.
# 2. Name objects with prefixes: "wall_", "floor_", "ceiling_"
# 3. Run this script to export scene layout

def export_scene_to_z1000():
    # Get output directory
    blend_dir = os.path.dirname(bpy.data.filepath)
    if not blend_dir:
        print("Save your Blender file first!")
        return
    
    output_path = os.path.join(blend_dir, "scene_export.json")
    
    scene_data = {
        "scene_name": bpy.context.scene.name,
        "objects": []
    }
    
    # Object type mapping
    type_prefixes = {
        "wall_": "wall",
        "floor_": "floor", 
        "ceiling_": "ceiling",
        "plane_": "plane",
        "cube_": "cube"
    }
    
    # Export all mesh objects
    for obj in bpy.context.scene.objects:
        if obj.type != 'MESH':
            continue
            
        # Determine object type from name
        obj_type = "wall"  # default
        for prefix, type_name in type_prefixes.items():
            if obj.name.lower().startswith(prefix):
                obj_type = type_name
                break
        
        # Get world transform
        loc = obj.location
        rot = obj.rotation_euler
        scale = obj.scale
        dims = obj.dimensions
        
        # Convert Blender coordinates to game coordinates
        # Blender: Z-up, Y-forward
        # Game: Y-up, Z-forward  
        game_pos = [loc.x, loc.z, -loc.y]
        game_rot = [rot.x * 57.2958, rot.z * 57.2958, -rot.y * 57.2958]  # radians to degrees
        game_scale = [scale.x, scale.z, scale.y]
        game_dims = [dims.x, dims.z, dims.y]
        
        obj_data = {
            "name": obj.name,
            "type": obj_type,
            "position": game_pos,
            "rotation": game_rot,
            "scale": game_scale,
            "dimensions": game_dims
        }
        
        scene_data["objects"].append(obj_data)
        print(f"Exported: {obj.name} as {obj_type}")
    
    # Write JSON file
    with open(output_path, 'w') as f:
        json.dump(scene_data, f, indent=2)
    
    print(f"Scene exported to: {output_path}")
    print(f"Exported {len(scene_data['objects'])} objects")

# Run the export
if __name__ == "__main__":
    export_scene_to_z1000()

# Add to Blender UI
class Z1000_OT_ExportScene(bpy.types.Operator):
    bl_idname = "export_scene.z1000"
    bl_label = "Export to Z1000 Engine"
    bl_description = "Export scene layout to Z1000 game engine"
    
    def execute(self, context):
        export_scene_to_z1000()
        self.report({'INFO'}, "Scene exported successfully")
        return {'FINISHED'}

class Z1000_PT_ExportPanel(bpy.types.Panel):
    bl_label = "Z1000 Engine Export"
    bl_idname = "PT_Z1000_export"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Z1000"
    
    def draw(self, context):
        layout = self.layout
        layout.operator("export_scene.z1000")

def register():
    bpy.utils.register_class(Z1000_OT_ExportScene)
    bpy.utils.register_class(Z1000_PT_ExportPanel)

def unregister():
    bpy.utils.unregister_class(Z1000_OT_ExportScene)
    bpy.utils.unregister_class(Z1000_PT_ExportPanel)

if __name__ == "__main__":
    register()