#pragma once
#ifndef _GEOMETRY_INTERFACE_PLANE_H_
#define _GEOMETRY_INTERFACE_PLANE_H_

#include "geometry_data_struct.h"

namespace TiGER
{
    void creatLine(std::array<double, 3> p0,
        std::array<double, 3> P1,
        TiGER::GeometryCurve& line);
    void creatPlane(std::array<double, 3> p0,
        std::array<double, 3> basis1,
        std::array<double, 3> basis2,
        GeometrySurface& plane);
}

#include "geometry_interface_plane.cpp"
#endif // !_GEOMETRY_INTERFACE_PLANE_H_