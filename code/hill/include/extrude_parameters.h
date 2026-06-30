 #pragma once
#ifndef _EXTRUDE_PARAMETERS_H_
#define _EXTRUDE_PARAMETERS_H_
#include "./parameters.h"
namespace TiGER
{
    class ExtrudeParameters : public APIPara
    {
    public:     
        struct GeometricProgressionPara
        {
            double initial_delta_s;
            double growth_rate;
            double minimum_delta_s;
            double maximum_delta_s;
        };
        struct StopCondition
        {
            int flag;
            enum stopConditionType
            {
                PositiveSkew = 0x001,
                NegativeSkew = 0x002,
                Zero = 0x004,
                Negative = 0x008,
                TotalHeight = 0x010,
                Skewness = 0x020,
                AspectRatio = 0x04
            };
            double stop_height;
            double stop_skewness;
            double stop_aspect_ratio;
        };
    public:
        DEFINE_SETTER(short, num_layer, ExtrudeParameters,1);
        DEFINE_SETTER(GeometricProgressionPara, geo_progre_para, ExtrudeParameters,GeometricProgressionPara());
        DEFINE_SETTER(bool, flip, ExtrudeParameters,false);
        DEFINE_SETTER(StopCondition, stop_condition, ExtrudeParameters,StopCondition());
    };

}

#endif // _EXTRUDE_PARAMETERS_H_