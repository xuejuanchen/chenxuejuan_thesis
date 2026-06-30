#pragma once
#ifndef _SIZE_FIELD_PARAMETERS_H_
#define _SIZE_FIELD_PARAMETERS_H_
#include <array>
#include "./parameters.h"
namespace TiGER
{
    struct BackGroundParameters : public APIPara
    {
        DEFINE_SETTER(double, hmax, BackGroundParameters,-1);
        DEFINE_SETTER(double, hmin, BackGroundParameters,-1);

        DEFINE_SETTER(double, theta, BackGroundParameters,0.1);
        DEFINE_SETTER(double, beta, BackGroundParameters,1.2);

        DEFINE_SETTER(double, proximity, BackGroundParameters,2);


    };

    struct SourceParameters : public APIPara
    {
        DEFINE_SETTER(double, beginSpacing, SourceParameters,10.9);
        DEFINE_SETTER(double, endSpacing, SourceParameters,0);

        DEFINE_SETTER(double, beginDecay, SourceParameters,0);
        DEFINE_SETTER(double, endDecay, SourceParameters,0);

        DEFINE_SETTER(double, sizing_value, SourceParameters,0);
    };
    struct PointSourceParameters : public SourceParameters
    {

        DEFINE_SETTER(double, inner_radius, PointSourceParameters,0);
        DEFINE_SETTER(double, outter_radius, PointSourceParameters,0);
        //   DEFINE_SETTER(std::array<double, 3>, xyz, PointSourceParameters);
    };
    struct LineSourceParameters : public SourceParameters
    {

        std::array<double, 2> inner_radius;
        std::array<double, 2> outter_radius;

        //DEFINE_SETTER(std::array<double, 2>, inner_radius, LineSourceParameters);
        //DEFINE_SETTER(std::array<double, 2>, outter_radius, LineSourceParameters);
        //   DEFINE_SETTER(std::array<std::array<double, 3>,2>, xyz, LineSourceParameters);

    };
    struct TriangleSourceParameters : public PointSourceParameters
    {
    };

}


#endif // _DISCRETIZE_CURVE_PARAMETERS_H_