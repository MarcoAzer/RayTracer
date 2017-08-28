#include <iostream>
#include <cmath>

#include <glm/ext.hpp>
#include <glm/gtx/io.hpp>

using namespace std;

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "Ray.hpp"
#include "PhongMaterial.hpp"

enum State
{
    nostate, background, shadow, color
};

glm::vec3 Get_Pixel_Color( SceneNode* root, 
                           const std::list<Light *> & lights, 
                           const glm::vec3 & ambient,
                           Ray& ray ){
    static bool first = true;

    State pixel_state = background;
    glm::vec3 col;
    float t = INT_MAX;
    SceneNode* obj_intersect = NULL;

    // Loop through all objects to find the one that intersects
    for( SceneNode* child : root->children )
    {
        float new_t = child->get_t( ray );

        // Intersect with object
        if( new_t > 0 )
        {
            // Object infront of another
            if( new_t < t )
            {
                t = new_t;
                obj_intersect = child;
            }
        }
    }

    // Intersect with object
    if( t != INT_MAX )
    {
        GeometryNode* g_node = (GeometryNode*)obj_intersect;
        PhongMaterial* phong = (PhongMaterial*)g_node->m_material;
        // Point of intersection
        glm::vec3 p = ray.origin + t * ray.dir;

        // Shadow Ray
        for( list<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++ )
        {
            glm::vec3 light_dir = p - (*it)->position;
            float light_dist = sqrt( light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z );
            light_dir = glm::normalize( light_dir );
            Ray light_ray( (*it)->position, light_dir );
            float light_t = root->nh_intersect( light_ray );

            const float epsilon = 0.05;

            glm::vec3 p1 = (*it)->position + light_t * light_dir;
            glm::vec3 diff = abs( p1 - p );

            glm::vec3 N = glm::normalize( g_node->m_primitive->normal( p ) );
            //p = p + epsilon * N;
                 
            // Test light intersection
            // Object between object and light
            if( diff.x < epsilon && diff.y < epsilon && diff.z < epsilon  ) 
            {
                pixel_state = color;

                // Lambertian
                float l_dot_n = glm::dot( -1 * light_dir, N ) / ( (*it)->falloff[0] + light_dist * (*it)->falloff[1] + light_dist * (*it)->falloff[2] * (*it)->falloff[2] );
                col.r = phong->m_kd.r * (*it)->colour.r * l_dot_n;
                col.g = phong->m_kd.g * (*it)->colour.g * l_dot_n;
                col.b = phong->m_kd.b * (*it)->colour.b * l_dot_n;

                // Spectral 
                float reflect = 2.0 * glm::dot( light_dir, N );
                glm::vec3 R = light_dir - reflect * N;
                float coeff = glm::dot( R, ray.dir );
                if( coeff < 0.0 )
                    coeff = 0.0;

                coeff = pow( coeff, phong->m_shininess );

                col.r += coeff * phong->m_ks.r * (*it)->colour.r;
                col.g += coeff * phong->m_ks.g * (*it)->colour.g;
                col.b += coeff * phong->m_ks.b * (*it)->colour.b;

                // Ambient
                col.r += phong->m_kd.r * ambient.r;
                col.g += phong->m_kd.g * ambient.g;
                col.b += phong->m_kd.b * ambient.b;
            }
            else
            {
                if( first ){
                    cout << endl;
                    cout << diff << endl;
                    first = false;
                }
                if( pixel_state != color )
                    pixel_state = shadow;
            }
        }
    }

    if( pixel_state == background )
        return glm::vec3( 0.9, 0.9, 0.9 );
    else if( pixel_state == shadow )
        return glm::vec3( 0.0, 0.0, 0.0 );
    else
        return col;
}

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights
) {

    glm::vec4 e( eye.x, eye.y, eye.z, 1 );
    e = glm::inverse( root->trans ) * e;
    glm::vec3 l_eye( e.x, e.y, e.z );
    
    glm::vec3 v_for = glm::normalize( view );
    glm::vec3 v_right = glm::normalize( glm::cross( v_for, up ) );
    glm::vec3 v_up = glm::cross( v_right, v_for ); // Already normalized

    const float h = 2 * tan( fovy / 2 );
    const float w = ( image.width() / image.height() ) * h;
    float fov_radius = fovy * M_PI / 360.0;

    const int background_section = image.height() / 3;

    cout << "Rendering " << image.width() << "x" << image.height() << " image." << endl;

    for( float i = 0; i < image.height(); i++ )
    {
        for( float j = 0; j < image.width(); j++ )
        {
            // Get direction vector
            glm::vec3 dir = v_for + h * ( ( ( 2 * i) / image.height() ) - 1 ) * v_up + w * ( ( ( 2 * j ) / image.width() ) - 1 ) * v_right;
            dir = glm::normalize( dir );

            // Intersection with each object
            Ray ray( l_eye, dir );

            glm::vec3 color = Get_Pixel_Color( root, lights, ambient, ray );

            image( j, i, 0 ) = color.r;
            image( j, i, 1 ) = color.g;
            image( j, i, 2 ) = color.b;

        }
        cout << "\r";
        cout << int(i / image.height() * 100) << "%" << std::flush;
    }
    cout << endl;

}
