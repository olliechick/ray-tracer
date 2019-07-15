/*----------------------------------------------------------
* COSC363  Ray Tracer
*
*  The Triangle class
*  This is a subclass of Object, and hence implements the
*  methods intersect() and normal().
-------------------------------------------------------------*/

#include "Triangle.h"
#include <math.h>


/**
* Checks if a point pt is inside the current polygon
* Implement a point inclusion test using 
* member variables a, b, c.
*/
bool Triangle::isInside(glm::vec3 pt)
{

	//=== Complete this function ====

    glm::vec3 ua = b-a;
    glm::vec3 ub = c-b;
    glm::vec3 uc = a-c;
    glm::vec3 va = pt-a;
    glm::vec3 vb = pt-b;
    glm::vec3 vc = pt-c;
    
    
    return glm::dot(glm::cross(ua, va), this->normal(a)) > 0 &&
    glm::dot(glm::cross(ub, vb), this->normal(a)) > 0 &&
    glm::dot(glm::cross(uc, vc), this->normal(a)) > 0;
}

/**
* Triangle's intersection method.  The input is a ray (pos, dir). 
*/
float Triangle::intersect(glm::vec3 posn, glm::vec3 dir)
{
	glm::vec3 n = normal(posn);
	glm::vec3 vdif = a - posn;
	float vdotn = glm::dot(dir, n);
	if(fabs(vdotn) < 1.e-4) return -1;
    float t = glm::dot(vdif, n)/vdotn;
	if(fabs(t) < 0.0001) return -1;
	glm::vec3 q = posn + dir*t;
	if(isInside(q)) return t;
    else return -1;
}

/**
* Returns the unit normal vector at a given point.
* Compute the triangle's normal vector using 
* member variables a, b, c.
* The parameter pt is a dummy variable and is not used.
*/
glm::vec3 Triangle::normal(glm::vec3 pt)
{
	glm::vec3 n = glm::vec3(0);
    
	//=== Complete this function ====

    n = normalize(glm::cross((b-a), (c-a)));

    return n;
}
