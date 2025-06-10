//
// Created by Adam Socki on 6/2/25.
//


struct WallEntity: Entity {
    vec3 position = V3(0, 0, 0);
    vec3 rotation = V3(0, 0, 0);
    vec3 scale = V3(1, 1, 1);

    // Render data (walls always render)
    Mesh* mesh;
    Material* material;
};


