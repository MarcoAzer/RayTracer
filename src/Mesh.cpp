#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

using namespace std;

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
    , intersected_triangle( NULL )
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;

	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

float
Mesh::get_t( Ray& ray )
{
    float t = INT_MAX;

    for( std::vector<Triangle>::iterator it = m_faces.begin(); it != m_faces.end(); it++ )
    {
        glm::vec3 R = ray.origin - m_vertices[it->v1];
        glm::vec3 vec_1 = m_vertices[it->v2] - m_vertices[it->v1];
        glm::vec3 vec_2 = m_vertices[it->v3] - m_vertices[it->v1];
        glm::vec3 vec_3 = -1 * ray.dir;

        float D = glm::determinant( glm::mat3( vec_1, vec_2, vec_3 ) );
        float D1 = glm::determinant( glm::mat3( R, vec_2, vec_3 ) );
        float D2 = glm::determinant( glm::mat3( vec_1, R, vec_3 ) );
        float D3 = glm::determinant( glm::mat3( vec_1, vec_2, R ) );

        float new_t = D3 / D;

        if( new_t > 0 && new_t < t )
        {
            t = new_t;
            intersected_triangle = &(*it);
        }
    }

    if( t != INT_MAX )
        return t;
    else
        return 0.0;
}

glm::vec3
Mesh::normal( glm::vec3& p )
{
    if( intersected_triangle )
    {
        Triangle* t = intersected_triangle ;
        glm::vec3 v1 = m_vertices[t->v1];
        glm::vec3 v2 = m_vertices[t->v2];
        glm::vec3 v3 = m_vertices[t->v3];

        return glm::cross( v2 - v1, v3 - v1 );
    }

    return glm::vec3(0);
}
