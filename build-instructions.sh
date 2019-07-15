#!/bin/bash

g++ -Wall -o "RayTracer" "RayTracer.cpp" Sphere.cpp Triangle.cpp Cone.cpp Cylinder.cpp Plane.cpp SceneObject.cpp Ray.cpp -lGL -lGLU -lglut
