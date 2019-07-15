/*========================================================================
* COSC 363  Computer Graphics (2018)
* Ray tracer 
* See Lab07.pdf for details.
*=========================================================================
*/
#include <iostream>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>
#include "Sphere.h"
#include "SceneObject.h"
#include "Ray.h"
#include "Plane.h"
#include "Cylinder.h"
#include "Cone.h"
#include "Triangle.h"
#include <GL/glut.h>
using namespace std;

const float WIDTH = 20.0;  
const float HEIGHT = 20.0;
const float EDIST = 40.0;
const int NUMDIV = 500;
const int MAX_STEPS = 8;
const float XMIN = -WIDTH * 0.5;
const float XMAX =  WIDTH * 0.5;
const float YMIN = -HEIGHT * 0.5;
const float YMAX =  HEIGHT * 0.5;
const float SHININESS = 50;

vector<SceneObject*> sceneObjects;  //A global list containing pointers to objects in the scene


//---The most important function in a ray tracer! ---------------------------------- 
//   Computes the colour value obtained by tracing a ray and finding its 
//     closest point of intersection with objects in the scene.
//----------------------------------------------------------------------------------
glm::vec3 trace(Ray ray, int step)
{
	glm::vec3 backgroundCol(0);
	glm::vec3 light(10, 40, -3);
	glm::vec3 ambientCol(0.2);   //Ambient color of light
    glm::vec3 specularCol(1);
    float eta = 1.01; //refractive index

    ray.closestPt(sceneObjects);		//Compute the closest point of intersection of objects with the ray

    if(ray.xindex == -1) return backgroundCol;      //If there is no intersection return background colour

    glm::vec3 normalVector = sceneObjects[ray.xindex]->normal(ray.xpt); // Compute normal vector of object at point of intersection
    glm::vec3 lightVector = normalize(light - ray.xpt); // Light vector (vector from point of intersection to light source)
    float lDotn = glm::dot(normalVector, lightVector); // Dot product: l.n

    glm::vec3 reflVector = glm::reflect(-lightVector, normalVector); // reflection vector (for specular reflections)
    float rdotv = glm::dot(reflVector, -ray.dir);
    float specularTerm;
    if (rdotv < 0) {
        specularTerm = 0;
    } else {
        specularTerm = glm::pow(rdotv, SHININESS);
    }

    // Check if there is an occlusion
    Ray shadow(ray.xpt, lightVector);
    shadow.closestPt(sceneObjects);
    
    glm::vec3 materialCol; //object's colour

    // Use chequered if a chequered plane
    Plane* planeptr = dynamic_cast<Plane*>(sceneObjects[ray.xindex]); //returns 0 if the object is not really a plane
    if (planeptr != 0) {
        // It is a plane
        materialCol = static_cast<Plane&>(sceneObjects[ray.xindex][0]).getColor(ray.xpt); //object's colour
    } else {
        materialCol = sceneObjects[ray.xindex]->getColor(ray.xpt); //object's colour
    }

    glm::vec3 colorSum;
    
    // Reflections
    if ((ray.xindex == 1 || ray.xindex == 2) && step < MAX_STEPS && normalVector[2] > 0) {
        glm::vec3 reflectedDir = glm::reflect(ray.dir, normalVector);
        Ray reflectedRay(ray.xpt, reflectedDir);
        glm::vec3 reflectedCol = trace(reflectedRay, step+1); // recursion
        colorSum = colorSum + (0.8f*reflectedCol);       
    }

    // Refractions
    if (ray.xindex == 3 && step < MAX_STEPS*2) {
        glm::vec3 refractedDir = glm::refract(ray.dir, normalVector, eta);
        Ray refractedRay1(ray.xpt, refractedDir);
        refractedRay1.closestPt(sceneObjects);
        glm::vec3 m = sceneObjects[ray.xindex]->normal(refractedRay1.xpt);
        glm::vec3 refractedDir2 = glm::refract(refractedDir, -1.0f*m, 1.0f/eta);
        
        Ray refractedRay2(refractedRay1.xpt, refractedDir2);
        glm::vec3 refractedCol = trace(refractedRay2, step+1); // recursion
        
        colorSum = colorSum + refractedCol;
    }
        

    if (lDotn <= 0 || (shadow.xindex > -1 && shadow.xdist < ray.xdist)) {
        // l.n is negative; point of intersection is in shadow
        return ambientCol*materialCol;
    } else {
        return ambientCol*materialCol + lDotn*materialCol + specularCol*specularTerm + colorSum;
    }
}

//---The main display module -----------------------------------------------------------
// In a ray tracing application, it just displays the ray traced image by drawing
// each cell as a quad.
//---------------------------------------------------------------------------------------
void display()
{
	float xp, yp;  //grid point
	float cellX = (XMAX-XMIN)/NUMDIV;  //cell width
	float cellY = (YMAX-YMIN)/NUMDIV;  //cell height

	glm::vec3 eye(0., 0., 0.);  //The eye position (source of primary rays) is the origin

	glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glBegin(GL_QUADS);  //Each cell is a quad.

	for(int i = 0; i < NUMDIV; i++)  	//For each grid point xp, yp
	{
		xp = XMIN + i*cellX;
		for(int j = 0; j < NUMDIV; j++)
		{
			yp = YMIN + j*cellY;

		    glm::vec3 dir(xp+0.5*cellX, yp+0.5*cellY, -EDIST);	//direction of the primary ray

		    Ray ray = Ray(eye, dir);		//Create a ray originating from the camera in the direction 'dir'
			ray.normalize();				//Normalize the direction of the ray to a unit vector
		    glm::vec3 col = trace (ray, 1); //Trace the primary ray and get the colour value

			glColor3f(col.r, col.g, col.b);
			glVertex2f(xp, yp);				//Draw each cell with its color value
			glVertex2f(xp+cellX, yp);
			glVertex2f(xp+cellX, yp+cellY);
			glVertex2f(xp, yp+cellY);
        }
    }

    glEnd();
    glFlush();
}


//---This function initializes the scene ------------------------------------------- 
//   Specifically, it creates scene objects (spheres, planes, cones, cylinders etc)
//     and add them to the list of scene objects.
//   It also initializes the OpenGL orthographc projection matrix for drawing the
//     the ray traced image.
//----------------------------------------------------------------------------------
void initialize()
{
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(XMIN, XMAX, YMIN, YMAX);
    glClearColor(0, 0, 0, 1);
    
    //-- Define colours
    
    glm::vec3 black(0, 0, 0);
    glm::vec3 red(1, 0, 0);
    glm::vec3 green(0, 1, 0);
    glm::vec3 blue(0, 0, 1);
    glm::vec3 cyan(0, 1, 1);
    glm::vec3 magenta(1, 0, 1);
    glm::vec3 yellow(1, 1, 0);
    glm::vec3 white(1, 1, 1);
    
    glm::vec3 dark_yellow(0.5, 0.5, 0);
    

	//-- Create pointers to objects
    
    // Three spheres
    
	Sphere *sphere1 = new Sphere(glm::vec3(0, 5.0, -150.0), 15.0, blue);
	Sphere *sphere2 = new Sphere(glm::vec3(-5.0, 10.0, -50.0), 4.0, magenta);
	Sphere *sphere3 = new Sphere(glm::vec3(-5, -10.0, -100.0), 5.0, cyan);

    // Stage/floor
    
    Plane *floor = new Plane (glm::vec3(-30., -20, -40), //a
                              glm::vec3(30., -20, -40),   //b
                              glm::vec3(30., -20, -200), //c
                              glm::vec3(-30., -20, -200),//d
                              dark_yellow, true);   //color (chequered)
                              
    // Box

    glm::vec3 boxcolour = red;
    
    // Box points
    glm::vec3 boxpointadf = glm::vec3(5., -5., -85);
    glm::vec3 boxpointabf = glm::vec3(15., -5., -85);
    glm::vec3 boxpointbcf = glm::vec3(15., -5., -95);
    glm::vec3 boxpointcdf = glm::vec3(5., -5., -95);

    glm::vec3 boxpointade = glm::vec3(5., 5., -85);
    glm::vec3 boxpointabe = glm::vec3(15., 5., -85);
    glm::vec3 boxpointbce = glm::vec3(15., 5., -95);
    glm::vec3 boxpointcde = glm::vec3(5., 5., -95);
    
    // Box sides
    Plane *boxsidea = new Plane (boxpointade, boxpointadf, boxpointabf, boxpointabe, boxcolour);
    Plane *boxsideb = new Plane (boxpointabe, boxpointabf, boxpointbcf, boxpointbce, boxcolour);
    Plane *boxsidec = new Plane (boxpointcde, boxpointbce, boxpointbcf, boxpointcdf, boxcolour);
    Plane *boxsided = new Plane (boxpointadf, boxpointade, boxpointcde, boxpointcdf, boxcolour);
    Plane *boxsidee = new Plane (boxpointade, boxpointcde, boxpointbce, boxpointabe, boxcolour);
    Plane *boxsidef = new Plane (boxpointadf, boxpointcdf, boxpointbcf, boxpointabf, boxcolour);

    // Cylinder

    glm::vec3 cylindercenter = glm::vec3(10, -20, -85);
    Cylinder *cylinder = new Cylinder(cylindercenter, 5., 5., yellow);
    
    // Cone
    
    glm::vec3 conecenter = glm::vec3(0, -20, -90);
    Cone *cone = new Cone(conecenter, 5., 10., green);

    // Pyramid

    glm::vec3 pyramidcolour = white;

    // Pyramid points
    glm::vec3 pypte = glm::vec3(-10, -10, -90);
    glm::vec3 pypta = glm::vec3(pypte[0]-5, pypte[1]-10, pypte[2]-5);
    glm::vec3 pyptb = glm::vec3(pypte[0]+5, pypte[1]-10, pypte[2]-5);
    glm::vec3 pyptc = glm::vec3(pypte[0]+5, pypte[1]-10, pypte[2]+5);
    glm::vec3 pyptd = glm::vec3(pypte[0]-5, pypte[1]-10, pypte[2]+5);

    // Pyramid sides
    Triangle *pyramidsidea = new Triangle(pyptb, pypta, pypte, pyramidcolour);
    Triangle *pyramidsideb = new Triangle(pyptc, pyptb, pypte, pyramidcolour);
    Triangle *pyramidsidec = new Triangle(pyptd, pyptc, pypte, pyramidcolour);
    Triangle *pyramidsided = new Triangle(pypta, pyptd, pypte, pyramidcolour);
    
    // Triangular prism
    glm::vec3 tprismcolour = white;
    
    float base_height = 5;
    float top_height = 10;
    
    glm::vec3 tpa = glm::vec3(10, base_height, -70);
    glm::vec3 tpb = glm::vec3(15, base_height, -70);
    glm::vec3 tpc = glm::vec3(10, base_height, -55);
    glm::vec3 tpd = glm::vec3(10, top_height, -70);
    glm::vec3 tpe = glm::vec3(15, top_height, -70);
    glm::vec3 tpf = glm::vec3(10, top_height, -55);
    
    Triangle *tprismbottom = new Triangle(tpa, tpb, tpc, tprismcolour);
    Plane *planea = new Plane(tpa, tpc, tpf, tpd, tprismcolour);
    Plane *planeb = new Plane(tpc, tpb, tpe, tpf, tprismcolour);
    Plane *planec = new Plane(tpa, tpd, tpe, tpb, tprismcolour);
    Triangle *tprismtop = new Triangle(tpd, tpf, tpe, tprismcolour);

	//--Add the above to the list of scene objects.
    
    sceneObjects.push_back(floor); // 1
   
    sceneObjects.push_back(sphere1); // 2
	sceneObjects.push_back(sphere2); // 3
	sceneObjects.push_back(sphere3); // 4
    
    sceneObjects.push_back(boxsidea); // 5
    sceneObjects.push_back(boxsideb); // 6
    sceneObjects.push_back(boxsidec); // 7
    sceneObjects.push_back(boxsided); // 8
    sceneObjects.push_back(boxsidee); // 9
    sceneObjects.push_back(boxsidef); // 10
    
    sceneObjects.push_back(cylinder); // 11
    sceneObjects.push_back(cone); // 12

    sceneObjects.push_back(pyramidsidea); // 13
    sceneObjects.push_back(pyramidsideb); // 14
    sceneObjects.push_back(pyramidsidec); // 15
    sceneObjects.push_back(pyramidsided); // 16
    
    sceneObjects.push_back(tprismbottom); // 17
    sceneObjects.push_back(planea); // 18
    sceneObjects.push_back(planeb); // 19
    sceneObjects.push_back(planec); // 20
    sceneObjects.push_back(tprismtop);
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(20, 20);
    glutCreateWindow("COSC363 Assignment 2: Ollie Chick");

    glutDisplayFunc(display);
    initialize();

    glutMainLoop();
    return 0;
}
