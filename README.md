# RayTracer
In this project I implement fully functional ray-tracer from scratch in C.
You can see the increments in the folder sequence: RT1 to RT4.
Each folder comes with a makefile.
To build, simply run make.
To run, ./main input-file-name
Here I am using my own input file format.
The full ray-tracer can be found in RT4 and implements:

-Spheres, ellipses, polygons
-Multiple light sources
-Texture maps
-Shadows
-Reflection
-Refraction
-Depth-of-field

Makefile written only for linux but all code is written generically so you could change gcc to your preffered compiler and it should theoretically build.
