#pragma once
#ifndef DISCRETIZE_CURVE_PARAMETERS_H_
#define DISCRETIZE_CURVE_PARAMETERS_H_
#include "./parameters.h"
namespace TiGER
{
    struct CurveParameters :public APIPara
    {
    public:
        enum SplineVariable
        {
            Arclength = 0,
            x,
            y,
            z
        } ;
        DEFINE_SETTER(bool, reverse, CurveParameters,false);
        DEFINE_SETTER(SplineVariable, splinevariable, CurveParameters,SplineVariable::Arclength);
    };

    struct CurveParametersDimension:public APIPara
    {
    public:
        DEFINE_SETTER(double, deviation, CurveParametersDimension,0);
        DEFINE_SETTER(double, angle, CurveParametersDimension,0);
        DEFINE_SETTER(double, delta_s, CurveParametersDimension,0);
        DEFINE_SETTER(int, dimension, CurveParametersDimension,0);
        DEFINE_SETTER(int, max_dimension, CurveParametersDimension,1024);
    };
    

    class CurveParametersShape : public CurveParameters
    {
    public:
        DEFINE_SETTER2(double, max_angle,0, double, max_deviation,0, ShapePara, CurveParametersShape);

    };

     class CurveParametersGeometric : public CurveParameters {
     public:
       DEFINE_SETTER(double, begin_s, CurveParametersGeometric, 0);
      DEFINE_SETTER(double, end_s, CurveParametersGeometric, 0);
       DEFINE_SETTER(int, dimension, CurveParametersGeometric, 0);
    };
    class CurveParametersTanh : public CurveParameters
    {
     public:
      DEFINE_SETTER(double, begin_s, CurveParametersTanh,0);
      DEFINE_SETTER(double, end_s, CurveParametersTanh,0);
      DEFINE_SETTER(int,dimension, CurveParametersTanh,0);
    };
    class CurveParametersAutomatic : public CurveParameters
    {
    };
    class CurveParametersGrowth : public CurveParameters
    {
    public:
        struct GrowthPara
        { // 这三个参数不能同时设置，只能设置其中两个，第三个必须设置为负数
            double total_height;
            double growth_rate;
            int number_of_layer;
        };

        DEFINE_SETTER(GrowthPara, begin, CurveParametersGrowth,GrowthPara());
        DEFINE_SETTER(GrowthPara, end, CurveParametersGrowth,GrowthPara());
        DEFINE_SETTER(double, middle_spacing, CurveParametersGrowth,0.5);
    };

}

#endif // _DISCRETIZE_CURVE_PARAMETERS_H_