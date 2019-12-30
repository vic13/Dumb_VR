
#pragma once

typedef struct  {
    glm::vec3 position;
    glm::vec3 color;
} PointLight;

typedef struct {
    float ka;
    float kd;
    float ks;
    float ns;
} Material;
