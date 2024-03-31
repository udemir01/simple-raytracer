CXX=gcc
CXXFLAGS= -O2 -I.

Raytracer: ray.o
	$(CXX) -o Raytracer ray.o
