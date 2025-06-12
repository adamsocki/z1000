"""
Blender Level Exporter for z1000 Game Engine
Export selected objects as level entities in JSON format

Usage:
1. Select objects in Blender that you want to export as entities
2. Run this script from Blender's text editor
3. Choose export location when prompted
4. Load the exported JSON file in your game engine with L key

Entity Type Mapping:
- Objects named with "wall_" prefix -> Wall entities
- Objects named with "light_" prefix -> Light entities  
- Objects named with "prop_" prefix -> Prop entities
- All other objects -> Wall entities (default)

Material Mapping:
- Uses Blender material name as materialName in JSON
- Falls back to "defaultMaterial" if no material assigned
"""

import bpy
import json
import bmesh
from mathutils import Vector
from bpy_extras.io_utils import ExportHelper
from bpy.props import StringProperty
from bpy.types import Operator

class LevelExporter(Operator, ExportHelper):
    """Export level data to JSON format"""
    bl_idname = "export_level.json"
    bl_label = "Export Level"
    
    filename_ext = ".json"
    
    filter_glob: StringProperty(
        default="*.json",
        options={'HIDDEN'},
        maxlen=255,
    )
    
    def execute(self, context):
        return self.export_level(context, self.filepath)
    
    def export_level(self, context, filepath):
        # Get selected objects or all objects if none selected
        objects = context.selected_objects if context.selected_objects else context.scene.objects
        
        if not objects:
            self.report({'WARNING'}, "No objects to export")
            return {'CANCELLED'}
        
        # Create level data structure
        level_data = {
            "levelName": bpy.path.display_name_from_filepath(filepath),
            "version": "1.0",
            "entities": []
        }
        
        entity_id = 0
        
        for obj in objects:
            # Skip non-mesh objects for now
            if obj.type != 'MESH':
                continue
                
            # Determine entity type from object name
            entity_type = self.get_entity_type(obj.name)
            
            # Create entity data
            entity = {
                "type": entity_type,
                "id": entity_id,
                "position": [obj.location.x, obj.location.z, -obj.location.y],  # Convert Y-up to Z-up
                "rotation": [obj.rotation_euler.x, obj.rotation_euler.z, -obj.rotation_euler.y],
                "scale": [obj.scale.x, obj.scale.z, obj.scale.y]
            }
            
            # Add material info
            material_name = "defaultMaterial"
            if obj.data.materials and obj.data.materials[0]:
                material_name = obj.data.materials[0].name
            entity["materialName"] = material_name
            
            # Add mesh name (use object name without prefix)
            mesh_name = self.clean_mesh_name(obj.name)
            entity["meshName"] = mesh_name
            
            # Add entity-specific properties
            if entity_type == "Wall":
                entity["isStatic"] = True
                entity["castsShadows"] = True
                entity["collisionRadius"] = max(obj.dimensions) / 2.0
                
            elif entity_type == "Light":
                # Get light properties if object has a light
                light_data = obj.data if obj.type == 'LIGHT' else None
                if light_data:
                    entity["lightType"] = self.convert_light_type(light_data.type)
                    entity["color"] = [light_data.color.r, light_data.color.g, light_data.color.b]
                    entity["intensity"] = light_data.energy
                    
                    if light_data.type == 'POINT':
                        entity["range"] = light_data.cutoff_distance if light_data.use_cutoff_distance else 10.0
                    elif light_data.type == 'SPOT':
                        entity["range"] = light_data.cutoff_distance if light_data.use_cutoff_distance else 10.0
                        entity["spotAngle"] = light_data.spot_size * 57.2958  # Convert radians to degrees
                else:
                    # Default light properties for mesh objects named as lights
                    entity["lightType"] = "Point"
                    entity["color"] = [1.0, 1.0, 1.0]
                    entity["intensity"] = 1.0
                    entity["range"] = 10.0
                    
            elif entity_type == "Prop":
                entity["isStatic"] = True
                entity["castsShadows"] = True
                entity["collisionRadius"] = max(obj.dimensions) / 2.0
            
            level_data["entities"].append(entity)
            entity_id += 1
        
        # Write JSON file
        try:
            with open(filepath, 'w') as f:
                json.dump(level_data, f, indent=2)
            
            self.report({'INFO'}, f"Exported {len(level_data['entities'])} entities to {filepath}")
            return {'FINISHED'}
            
        except Exception as e:
            self.report({'ERROR'}, f"Failed to write file: {str(e)}")
            return {'CANCELLED'}
    
    def get_entity_type(self, obj_name):
        """Determine entity type from object name"""
        name_lower = obj_name.lower()
        
        if name_lower.startswith("wall_"):
            return "Wall"
        elif name_lower.startswith("light_"):
            return "Light"  
        elif name_lower.startswith("prop_"):
            return "Prop"
        else:
            return "Wall"  # Default fallback
    
    def clean_mesh_name(self, obj_name):
        """Remove type prefixes from object name to get clean mesh name"""
        prefixes = ["wall_", "light_", "prop_"]
        name_lower = obj_name.lower()
        
        for prefix in prefixes:
            if name_lower.startswith(prefix):
                return obj_name[len(prefix):]
        
        return obj_name
    
    def convert_light_type(self, blender_light_type):
        """Convert Blender light type to engine light type"""
        if blender_light_type == 'SUN':
            return "Directional"
        elif blender_light_type == 'POINT':
            return "Point"
        elif blender_light_type == 'SPOT':
            return "Spot"
        else:
            return "Point"  # Default fallback

def menu_func_export(self, context):
    self.layout.operator(LevelExporter.bl_idname, text="Level (.json)")

def register():
    bpy.utils.register_class(LevelExporter)
    bpy.types.TOPBAR_MT_file_export.append(menu_func_export)

def unregister():
    bpy.utils.unregister_class(LevelExporter)
    bpy.types.TOPBAR_MT_file_export.remove(menu_func_export)

if __name__ == "__main__":
    register()
    
    # Run the export dialog when script is executed directly
    bpy.ops.export_level.json('INVOKE_DEFAULT')