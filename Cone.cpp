/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cone class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cone.h"
#include <math.h>

/**
* Cone's intersection method.  The input is a ray (pos, dir).
*/
float Cone::intersect(glm::vec3 posn, glm::vec3 dir)
{    
    float m = (radius*radius) / (height*height);
    glm::vec3 vertex = glm::vec3(center[0], center[1] + height, center[2]);
    glm::vec3 w = posn - vertex;
    
    float t0, t1;
    float a = glm::dot(dir, dir) - m*dir[1]*dir[1] - dir[1]*dir[1];
    float b = 2 * ( glm::dot(dir, w) - m*dir[1]*w[1] - dir[1]*w[1] );
    float c = glm::dot(w, w) - m*w[1]*w[1] - w[1]*w[1];

    float discriminant = b*b - 4*a*c;
    if (discriminant<0.001)
        return -1;
        
    float sqrtdisc = sqrt(discriminant);
    t0 = (-b + sqrtdisc) / (2 * a);
    t1 = (-b - sqrtdisc) / (2 * a);

    float y0 = posn[1] + t0 * dir[1];
    float y1 = posn[1] + t1 * dir[1];

    float base_y = center[1];
    float top_y = center[1] + height;
    
    if (y0 < base_y) {
        // Started below base
        if (y1 < base_y){
            // went under base
            return -1;
        } else {
            // entered from base
            return t1;
        }
    } else if (y0 > top_y) {
        // Started above top
        // It cannot possibly enter from the top
        return -1;
    } else {
        // Started by entering
        return (t0 < t1)? t0: t1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cone.
*/
glm::vec3 Cone::normal(glm::vec3 p)
{
    // Find "actual" center (Cone's center is actually the center of the base)
    glm::vec3 center_at_p_height = glm::vec3(center[0], p[1], center[2]);
    glm::vec3 n = p - center_at_p_height;
    
    n = glm::normalize(n);
    return n;
}
