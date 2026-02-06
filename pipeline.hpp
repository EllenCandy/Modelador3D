#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "math.hpp"

Matriz4x4 montaPipeline(
    Vec3 VRP, Vec3 P, Vec3 Y_up,
    float xmin, float xmax, float ymin, float ymax,
    float umin, float umax, float vmin, float vmax,
    float Near, float Far
);

struct CameraConfig {
    Vec3 VRP;
    Vec3 P;
    Vec3 Y_up;
    float xmin, xmax, ymin, ymax;
    float umin, umax, vmin, vmax;
    float Near, Far;
};

Matriz4x4 calculaMatrizPipeline(CameraConfig c);

Vec4 aplicaPipeline(Matriz4x4 M, Vec4 p);

#endif
