//
// Created by Adam Socki on 6/4/25.
//



struct MeshFactory {
    DynamicArray<Mesh> meshes;
    std::unordered_map<std::string, Mesh*> meshNamePointerMap;

    std::vector<std::string> availableMeshNames;
};

