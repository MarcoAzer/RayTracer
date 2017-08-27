#include <iostream>
#include "Primitive.hpp"
#include "polyroots.hpp"

#include <glm/gtx/io.hpp>

using namespace std;

glm::vec3
Primitive::normal( glm::vec3& p )
{
    return glm::vec3(0);
}
  
bool 
Primitive::epsilon_check( glm::vec3& p )
{
    return false;
}

Primitive::~Primitive()
{
}

float
Sphere::get_t( Ray& ray )
{
    return 0.0;
}

Sphere::~Sphere()
{
}

float
Cube::get_t( Ray& ray )
{
    return 0.0;
}

Cube::~Cube()
{
}

float
NonhierSphere::get_t( Ray& ray )
{
    glm::vec3 dist = ray.origin - m_pos;

    float A = glm::dot( ray.dir, ray.dir );
    float B = glm::dot( ray.dir, dist ) * 2;
    float C = glm::dot( dist, dist ) - ( m_radius * m_radius );

    double roots[2];
    size_t roots_num = quadraticRoots( A, B, C, roots );

    if( roots_num == 0 )
    {
        //cout << "Zero roots" << endl;
        return 0;
    }
    else if( roots[0] > 0.001 && roots[0] < roots[1] )
    {
        //cout << "Root[0]: " << roots[0] << endl;
        return roots[0];
    }
    else if( roots[1] > 0.001 && roots[0] > roots[1] )
    {
        //cout << "Root[1]: " << roots[1] << endl;
        return roots[1];
    }
    else
    {
        //cout << "Default Case: " << roots[0] << " " << roots[1] << endl;
        return 0;
    }
}

glm::vec3
NonhierSphere::normal( glm::vec3& p )
{
    return p - m_pos;
}

bool 
NonhierSphere::epsilon_check( glm::vec3& p )
{
    if( glm::dot( p - m_pos, p - m_pos ) - ( m_radius * m_radius ) > 0 )
        return true;
    else
        return false;
}

NonhierSphere::~NonhierSphere()
{
}

glm::vec3
NonhierPlane::get_normal()
{
    return glm::cross( v2 - v1, v3 - v1 );
}

NonhierBox::NonhierBox( const glm::vec3& pos, double size ): m_pos( pos ), m_size( size ), intersected_plane( -1 )
{
    vers[0] = pos;
    vers[1] = glm::vec3( m_pos.x, m_pos.y + m_size, m_pos.z );
    vers[2] = glm::vec3( m_pos.x + m_size, m_pos.y, m_pos.z );
    vers[3] = glm::vec3( m_pos.x + m_size, m_pos.y, m_pos.z + m_size );
    vers[4] = glm::vec3( m_pos.x + m_size, m_pos.y + m_size, m_pos.z + m_size );
    vers[5] = glm::vec3( m_pos.x, m_pos.y, m_pos.z + m_size );
    vers[6] = glm::vec3( m_pos.x, m_pos.y + m_size, m_pos.z + m_size );
    vers[7] = glm::vec3( m_pos.x + m_size, m_pos.y + m_size, m_pos.z );

    planes[0] = NonhierPlane( vers[0], vers[1], vers[2] );
    planes[1] = NonhierPlane( vers[3], vers[4], vers[5] );
    planes[2] = NonhierPlane( vers[5], vers[6], vers[0] );
    planes[3] = NonhierPlane( vers[2], vers[7], vers[3] );
    planes[4] = NonhierPlane( vers[1], vers[6], vers[7] );
    planes[5] = NonhierPlane( vers[2], vers[3], vers[0] );
}

float
NonhierBox::get_t( Ray& ray )
{
    float t = INT_MAX;

    for( int i = 0; i < 6; i++ )
    {
        glm::vec3 N = glm::normalize( planes[i].get_normal() );
        float d_dot_n = glm::dot( ray.dir, N );
        if( d_dot_n != 0 ) // Intersect with infinite plane
        {
            float new_t = glm::dot( planes[i].v1 - ray.origin, N ) / d_dot_n;
            if( new_t ) // Inside
            {
                glm::vec3 intersect = ray.origin + new_t * ray.dir; 
                if( intersect.x >= m_pos.x && 
                    intersect.y >= m_pos.y &&
                    intersect.z >= m_pos.z &&
                    intersect.x <= m_pos.x + m_size &&
                    intersect.y <= m_pos.y + m_size &&
                    intersect.z <= m_pos.z + m_size )
                {
                    if( new_t > 0 && new_t < t )
                    {
                        intersected_plane = i;
                        t = new_t;
                    }
                }
            }
        }
    }

    if( t != INT_MAX )
        return t;
    else
        return 0.0;
}

glm::vec3
NonhierBox::normal( glm::vec3& p )
{
    glm::vec3 v(0);
    if( intersected_plane >= 0 && intersected_plane < 6 )
    {
        v = planes[intersected_plane].get_normal();
        intersected_plane = -1;
    }

    return v;
}

bool 
NonhierBox::epsilon_check( glm::vec3& p )
{
    if( p.x >= m_pos.x && 
        p.y >= m_pos.y &&
        p.z >= m_pos.z &&
        p.x <= m_pos.x + m_size &&
        p.y <= m_pos.y + m_size &&
        p.z <= m_pos.z + m_size )
    {
        return true;
    }
    else
        return false;
}

NonhierBox::~NonhierBox()
{
}
