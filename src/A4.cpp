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
    
    // Fill in raytracing code here...
    glm::vec3 v_for = glm::normalize( view );
    //glm::vec3 v_for(0, 0, 1);
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
             // dir = f + (h * y) * up + (w * x) * right
            glm::vec3 dir = v_for + h * ( ( ( 2 * i) / image.height() ) - 1 ) * v_up + w * ( ( ( 2 * j ) / image.width() ) - 1 ) * v_right;
            dir = glm::normalize( dir );
            State pixel_state = background;

            // Intersection with each object
            glm::vec3 col;
            Ray ray( l_eye, dir );
            float t = INT_MAX;
            SceneNode* obj_intersect = NULL;

            // Loop through all objects to find the one that intersects
            for( SceneNode* child : root->children )
            {
                float new_t = child->get_t( ray );

                if( new_t > 0 && new_t < t )
                {
                    t = new_t;
                    obj_intersect = child;
                }
            }

            if( t == INT_MAX )
            {
                pixel_state = background;
            }
            else
            {
                GeometryNode* g_node = (GeometryNode*)obj_intersect;
                PhongMaterial* phong = (PhongMaterial*)g_node->m_material;
                glm::vec3 p = ray.origin + t * ray.dir;

                for( list<Light*>::const_iterator it = lights.begin(); it != lights.end(); it++ )
                {
                    glm::vec3 light_dir = p - (*it)->position;
                    float light_dist = sqrt( light_dir.x * light_dir.x + light_dir.y * light_dir.y + light_dir.z * light_dir.z );
                    light_dir = glm::normalize( light_dir );
                    Ray light_ray( (*it)->position, light_dir );
                    float light_t = root->nh_intersect( light_ray );

                    const float epsilon = 0.01;

                    glm::vec3 N = glm::normalize( g_node->m_primitive->normal( p ) );
                    p = p + epsilon * N;
                         
                    // Test light intersection
                    // Object between object and light
                    if( light_t == 0 )
                    {
                    }
                    else if( abs( light_t - light_dist ) < epsilon /*|| g_node->m_primitive->epsilon_check( p )*/ ) 
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
                        float coeff = glm::dot( R, dir );
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
                        if( pixel_state != color )
                            pixel_state = shadow;
                    }
                }
            }

            if( pixel_state == background )
            {
                if( i < background_section )
                {
                    image( j, i, 0 ) = 0.0 + ( 2 * i / 255 );
                    image( j, i, 1 ) = 0.0 + ( 2 * i / 255 );
                    image( j, i, 2 ) = 0.5 + ( 2 * i / 255 );
                }
                else if( i < background_section * 2 )
                {
                    image( j, i, 0 ) = 0.0 + ( 2 * i / 255 );
                    image( j, i, 1 ) = 0.0 + ( 2 * i / 255 );
                    image( j, i, 2 ) = 0.5 + ( 2 * i / 255 );
                }
                else
                {
                    image( j, i, 0 ) = 139.0/255.0 + ( 255.0 - i )/ 255.0;
                    image( j, i, 1 ) =  69.0/255.0 + ( 255.0 - i )/ 255.0;
                    image( j, i, 2 ) =  19.0/255.0 + ( 255.0 - i )/ 255.0;
                }
            }
            else if( pixel_state == shadow )
            {
                image( j, i, 0 ) = 0.0;
                image( j, i, 1 ) = 0.0;
                image( j, i, 2 ) = 0.0;
            }
            else
            {
                image( j, i, 0 ) = col.r;
                image( j, i, 1 ) = col.g;
                image( j, i, 2 ) = col.b;
            }
        }
        cout << "\r";
        cout << int(i / image.height() * 100) << "%" << std::flush;
    }
    cout << endl;

    //std::cout << "Calling A4_Render(\n" <<
	//	  "\t" << *root <<
    //      "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
    //      "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
	//	  "\t" << "view: " << glm::to_string(view) << std::endl <<
	//	  "\t" << "up:   " << glm::to_string(up) << std::endl <<
	//	  "\t" << "fovy: " << fovy << std::endl <<
    //      "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
	//	  "\t" << "lights{" << std::endl;

	//for(const Light * light : lights) {
	//	std::cout << "\t\t" <<  *light << std::endl;
	//}
	//std::cout << "\t}" << std::endl;
	//std:: cout <<")" << std::endl;

	//size_t height = image.height();
	//size_t width = image.width();

	//for (uint y = 0; y < height; ++y) {
	//	for (uint x = 0; x < width; ++x) {
	//		// Red: increasing from top to bottom
	//		image(x, y, 0) = (double)y / height;
	//		// Green: increasing from left to right
	//		image(x, y, 1) = (double)x / width;
	//		// Blue: in lower-left and upper-right corners
	//		image(x, y, 2) = ((y < height/2 && x < width/2)
	//					  || (y >= height/2 && x >= width/2)) ? 1.0 : 0.0;
	//	}
	//}

}
