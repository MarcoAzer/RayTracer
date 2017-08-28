#pragma once

#include "Ray.hpp"
#include <glm/glm.hpp>

class Primitive {
public:
  virtual float get_t( Ray& ) = 0;
  virtual glm::vec3 normal( glm::vec3& p );
  virtual bool epsilon_check( glm::vec3& p );
  virtual ~Primitive();
};

class Sphere : public Primitive {
public:
  virtual float get_t( Ray& );
  virtual ~Sphere();
};

class Cube : public Primitive {
public:
  virtual float get_t( Ray& );
  virtual ~Cube();
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual float get_t( Ray& );
  virtual glm::vec3 normal( glm::vec3& p );
  virtual bool epsilon_check( glm::vec3& p );
  virtual ~NonhierSphere();

  double m_radius;
private:
  glm::vec3 m_pos;
};

class NonhierPlane {
public:
    NonhierPlane(): v1(0.0), v2(0.0), v3(0.0)
    {
    }
    NonhierPlane( glm::vec3& v1, glm::vec3& v2, glm::vec3& v3 )
        : v1(v1), v2(v2), v3(v3)
    {
    }
    
    glm::vec3 get_normal();

    glm::vec3 v1, v2, v3;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size);
  virtual float get_t( Ray& );
  virtual glm::vec3 normal( glm::vec3& p );
  virtual bool epsilon_check( glm::vec3& p );
  virtual ~NonhierBox();

private:
  glm::vec3 m_pos;
  double m_size;
  glm::vec3 vers[8];
  NonhierPlane planes[6];
  int intersected_plane;
};
