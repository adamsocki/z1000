import bpy
import json
import os
from mathutils import Vector, Matrix
import struct

# Blender to Z1000 Engine Export Script
# Usage:
# 1. Create meshes in Blender (cubes, planes, custom geometry)
# 2. Name objects with prefixes: "wall_", "floor_", "ceiling_", "prop_"
# 3. Apply materials/textures if desired
# 4. Run this script to export scene layout and geometry

class Z1000Exporter:
    def __init__(self):
        self.scene_data = {
            "scene_name": bpy.context.scene.name,
            "objects": [],
            "meshes": {},
            "materials": {}
        }

    def export_scene(self):
        """Main export function"""
        # Get output directory
        blend_dir = os.path.dirname(bpy.data.filepath)
        if not blend_dir:
            print("Save your Blender file first!")
            return False

        # Create export directory
        export_dir = os.path.join(blend_dir, "z1000_export")
        os.makedirs(export_dir, exist_ok=True)

        # Export all mesh objects
        for obj in bpy.context.scene.objects:
            if obj.type != 'MESH':
                continue

            self.export_object(obj)

        # Write JSON file
        json_path = os.path.join(export_dir, "scene_data.json")
        with open(json_path, 'w') as f:
            json.dump(self.scene_data, f, indent=2)

        # Generate C++ code
        cpp_path = os.path.join(export_dir, "scene_loader.cpp")
        self.generate_cpp_code(cpp_path)

        print(f"Scene exported to: {export_dir}")
        print(f"Exported {len(self.scene_data['objects'])} objects")
        print(f"Exported {len(self.scene_data['meshes'])} unique meshes")

        return True

    def export_object(self, obj):
        """Export a single object"""
        # Determine object type from name
        obj_type = self.get_object_type(obj.name)

        # Get world transform (Z-up coordinate system like your engine)
        loc = obj.location
        rot = obj.rotation_euler
        scale = obj.scale

        # Get mesh data
        mesh_key = self.export_mesh(obj)

        # Get material data
        material_key = None
        if obj.data.materials:
            material_key = self.export_material(obj.data.materials[0])

        obj_data = {
            "name": obj.name,
            "type": obj_type,
            "position": [loc.x, loc.y, loc.z],
            "rotation": [rot.x * 57.2958, rot.y * 57.2958, rot.z * 57.2958],  # to degrees
            "scale": [scale.x, scale.y, scale.z],
            "mesh": mesh_key,
            "material": material_key
        }

        self.scene_data["objects"].append(obj_data)
        print(f"Exported: {obj.name} as {obj_type}")

    def get_object_type(self, name):
        """Determine object type from name"""
        type_prefixes = {
            "wall_": "wall",
            "floor_": "floor",
            "ceiling_": "ceiling",
            "prop_": "prop",
            "plane_": "plane",
            "cube_": "cube"
        }

        name_lower = name.lower()
        for prefix, type_name in type_prefixes.items():
            if name_lower.startswith(prefix):
                return type_name

        return "prop"  # default

    def export_mesh(self, obj):
        """Export mesh vertex data"""
        mesh = obj.data
        mesh_key = f"{mesh.name}_{len(mesh.vertices)}v_{len(mesh.polygons)}f"

        # Skip if already exported
        if mesh_key in self.scene_data["meshes"]:
            return mesh_key

        # Apply modifiers and get final mesh
        depsgraph = bpy.context.evaluated_depsgraph_get()
        obj_eval = obj.evaluated_get(depsgraph)
        mesh_eval = obj_eval.to_mesh()

        # Triangulate mesh
        import bmesh
        bm = bmesh.new()
        bm.from_mesh(mesh_eval)
        bmesh.ops.triangulate(bm, faces=bm.faces[:])
        bm.to_mesh(mesh_eval)
        bm.free()

        # Calculate normals
        mesh_eval.calc_normals()

        # Export vertices
        vertices = []
        for vert in mesh_eval.vertices:
            vertices.append({
                "pos": [vert.co.x, vert.co.y, vert.co.z],
                "normal": [vert.normal.x, vert.normal.y, vert.normal.z]
            })

        # Export faces with UV coordinates
        indices = []
        vertex_data = []  # Final vertex data with UVs
        vertex_map = {}  # Map from (vert_idx, uv) to final index

        uv_layer = mesh_eval.uv_layers.active

        for poly in mesh_eval.polygons:
            face_indices = []

            for loop_idx in poly.loop_indices:
                loop = mesh_eval.loops[loop_idx]
                vert_idx = loop.vertex_index

                # Get UV coordinates
                uv = [0.5, 0.5]  # default
                if uv_layer:
                    uv_data = uv_layer.data[loop_idx]
                    uv = [uv_data.uv.x, 1.0 - uv_data.uv.y]  # Flip V coordinate

                # Create unique vertex for each vert+uv combination
                key = (vert_idx, tuple(uv))
                if key not in vertex_map:
                    vert = vertices[vert_idx]
                    vertex_map[key] = len(vertex_data)
                    vertex_data.append({
                        "pos": vert["pos"],
                        "normal": vert["normal"],
                        "uv": uv,
                        "color": [1.0, 1.0, 1.0]  # default white
                    })

                face_indices.append(vertex_map[key])

            # Add triangle indices (should already be triangulated)
            if len(face_indices) == 3:
                indices.extend(face_indices)

        # Store mesh data
        self.scene_data["meshes"][mesh_key] = {
            "vertices": vertex_data,
            "indices": indices,
            "vertex_count": len(vertex_data),
            "index_count": len(indices)
        }

        # Clean up
        obj_eval.to_mesh_clear()

        return mesh_key

    def export_material(self, material):
        """Export material data"""
        mat_key = material.name

        if mat_key in self.scene_data["materials"]:
            return mat_key

        mat_data = {
            "name": material.name,
            "type": "PBR",
            "texture": None
        }

        # Try to find texture from material nodes
        if material.use_nodes:
            for node in material.node_tree.nodes:
                if node.type == 'TEX_IMAGE' and node.image:
                    mat_data["texture"] = {
                        "filename": os.path.basename(node.image.filepath),
                        "path": node.image.filepath
                    }
                    break

        self.scene_data["materials"][mat_key] = mat_data
        return mat_key

    def generate_cpp_code(self, output_path):
        """Generate C++ code for loading the scene"""
        code = []
        code.append("// Auto-generated scene loader for Z1000 Engine")
        code.append("// Generated from: " + bpy.context.scene.name)
        code.append("")
        code.append("void LoadBlenderScene(Zayn* zaynMem) {")
        code.append("    MemoryArena* arena = &zaynMem->permanentMemory;")
        code.append("    ")

        # Generate mesh creation code
        code.append("    // Create meshes")
        mesh_vars = {}
        for i, (mesh_key, mesh_data) in enumerate(self.scene_data["meshes"].items()):
            var_name = f"mesh_{i}"
            mesh_vars[mesh_key] = var_name

            code.append(f"    Mesh {var_name} = {{}};")
            code.append(f"    {var_name}.name = \"{mesh_key}\";")
            code.append(f"    ")

            # Add vertices
            code.append(f"    // Vertices for {mesh_key}")
            code.append(f"    {var_name}.vertices = {{")

            for v in mesh_data["vertices"]:
                pos = v["pos"]
                norm = v["normal"]
                uv = v["uv"]
                color = v["color"]
                code.append(f"        {{{{{pos[0]}f, {pos[1]}f, {pos[2]}f}}, "
                            f"{{{color[0]}f, {color[1]}f, {color[2]}f}}, "
                            f"{{{uv[0]}f, {uv[1]}f}}, "
                            f"{{{norm[0]}f, {norm[1]}f, {norm[2]}f}}}},")

            code.append("    };")
            code.append("    ")

            # Add indices
            code.append(f"    // Indices for {mesh_key}")
            code.append(f"    {var_name}.indices = {{")

            # Group indices by 12 per line for readability
            indices = mesh_data["indices"]
            for i in range(0, len(indices), 12):
                line = "        "
                line += ", ".join(str(idx) for idx in indices[i:i+12])
                if i + 12 < len(indices):
                    line += ","
                code.append(line)

            code.append("    };")
            code.append("    ")

            # Create vertex and index buffers
            code.append(f"    CreateVertexBuffer(&zaynMem->renderer, {var_name}.vertices, "
                        f"&{var_name}.vertexBuffer, &{var_name}.vertexBufferMemory);")
            code.append(f"    CreateIndexBuffer(&zaynMem->renderer, {var_name}.indices, "
                        f"&{var_name}.indexBuffer, &{var_name}.indexBufferMemory);")
            code.append("    ")

            # Store mesh
            code.append(f"    uint32_t {var_name}_idx = PushBack(&zaynMem->meshFactory.meshes, {var_name});")
            code.append(f"    Mesh* {var_name}_ptr = &zaynMem->meshFactory.meshes[{var_name}_idx];")
            code.append(f"    EnableMeshInstancing(zaynMem, {var_name}_ptr, 100);")
            code.append("    ")

        # Generate entity creation code
        code.append("    // Create entities")
        for obj in self.scene_data["objects"]:
            if obj["type"] == "wall":
                code.append(f"    // {obj['name']}")
                code.append("    EntityHandle wall_handle = AddEntity(&zaynMem->entityFactory, EntityType_Wall);")
                code.append("    PushBack(&zaynMem->gameData.walls, wall_handle);")
                code.append("    WallEntity* wall = (WallEntity*)GetEntity(&zaynMem->entityFactory, wall_handle);")

                # Set mesh
                if obj["mesh"] in mesh_vars:
                    code.append(f"    wall->mesh = {mesh_vars[obj['mesh']]}_ptr;")

                # Set transform
                pos = obj["position"]
                rot = obj["rotation"]
                scale = obj["scale"]
                code.append(f"    mat4 transform = TRS(V3({pos[0]}f, {pos[1]}f, {pos[2]}f), "
                            f"V3({rot[0]}f, {rot[1]}f, {rot[2]}f), "
                            f"V3({scale[0]}f, {scale[1]}f, {scale[2]}f));")
                code.append(f"    AddMeshInstance(wall->mesh, wall_handle, transform);")
                code.append("    ")

        code.append("}")

        # Write the code
        with open(output_path, 'w') as f:
            f.write('\n'.join(code))

# Blender UI Integration
class Z1000_OT_ExportScene(bpy.types.Operator):
    bl_idname = "export_scene.z1000"
    bl_label = "Export to Z1000 Engine"
    bl_description = "Export scene layout and geometry to Z1000 game engine"

    def execute(self, context):
        exporter = Z1000Exporter()
        if exporter.export_scene():
            self.report({'INFO'}, "Scene exported successfully")
            return {'FINISHED'}
        else:
            self.report({'ERROR'}, "Export failed")
            return {'CANCELLED'}

class Z1000_PT_ExportPanel(bpy.types.Panel):
    bl_label = "Z1000 Engine Export"
    bl_idname = "PT_Z1000_export"
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'UI'
    bl_category = "Z1000"

    def draw(self, context):
        layout = self.layout

        col = layout.column()
        col.label(text="Export Options:")
        col.operator("export_scene.z1000", text="Export Scene", icon='EXPORT')

        col.separator()
        col.label(text="Naming Convention:")
        col.label(text="• wall_ : Wall entities")
        col.label(text="• floor_ : Floor entities")
        col.label(text="• ceiling_ : Ceiling entities")
        col.label(text="• prop_ : Generic props")

def register():
    bpy.utils.register_class(Z1000_OT_ExportScene)
    bpy.utils.register_class(Z1000_PT_ExportPanel)

def unregister():
    bpy.utils.unregister_class(Z1000_OT_ExportScene)
    bpy.utils.unregister_class(Z1000_PT_ExportPanel)

if __name__ == "__main__":
    register()
    # Run export immediately if script is run directly
    exporter = Z1000Exporter()
    exporter.export_scene()