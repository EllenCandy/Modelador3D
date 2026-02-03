#ifndef PIPELINE_HPP
#define PIPELINE_HPP

#include "math.hpp"

Matriz4x4 montaPipeline(
    Vec3 VRP, Vec3 P, Vec3 Y_up,
    float xmin, float xmax, float ymin, float ymax,
    float umin, float umax, float vmin, float vmax,
    float Near, float Far
);

#endif
