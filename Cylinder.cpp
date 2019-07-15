/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The cylinder class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Cylinder.h"
#include <math.h>

/**
* Cylinder's intersection method.  The input is a ray (pos, dir). 
*/
float Cylinder::intersect(glm::vec3 posn, glm::vec3 dir)
{
    float t0, t1;
    float a = dir[0]*dir[0] + dir[2]*dir[2];
    float b = 2 * (dir[0] * (posn[0]-center[0]) + dir[2] * (posn[2]-center[2]));
    float c = (posn[0] - center[0])*(posn[0] - center[0]) + (posn[2] - center[2])*(posn[2] - center[2]) - radius*radius;

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
        if (y1 > top_y) {
            // went over top
            return -1;
        } else {
            // entered from top
            return t1;
        }
    } else {
        // Started by entering
        return (t0 < t1)? t0: t1;
    }
}

/**
* Returns the unit normal vector at a given point.
* Assumption: The input point p lies on the cylinder.
*/
glm::vec3 Cylinder::normal(glm::vec3 p)
{
    if (p[1] > center[1] + height)
        // Top cap
        return glm::vec3(0, 1, 0);
    else if (p[1] <= center[1])
        // Bottom cap
        return glm::vec3(0, -1, 0);
    
    glm::vec3 center_at_p_height = glm::vec3(center[0], p[1], center[2]);
    glm::vec3 n = p - center_at_p_height;
    n = glm::normalize(n);
    return n;
}
