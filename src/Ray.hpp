#pragma once

#include <glm/ext.hpp>

class Ray
{
    public:
        glm::vec3 origin;
        glm::vec3 dir;
        Ray( glm::vec3 origin, glm::vec3 dir );
};
