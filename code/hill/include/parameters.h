#pragma once
#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_
#include "./mesh_data_struct.h"
#include "./definitions.h"
namespace TiGER
{
    class APIPara
    {
    public:
        APIPara &setParam1(int value)
        {
            // Set param1
            return *this;
        }

        APIPara &setParam2(double value)
        {
            // Set param2
            return *this;
        }

        // ... other setters for parameters

        APIPara build()
        {
            // Return the constructed parameters object
            return APIPara{/* initialized values */};
        }
    };

}
#define DEFINE_SETTER(variableType, variableName,className,default_value) \
    public:\
    virtual className& set##variableName(variableType value) { \
        this->variableName = value; \
        return *this;\
    }\
    \
    virtual const variableType& get##variableName() const { \
        return variableName;\
    }\
    protected:\
    variableType variableName = default_value;\




#define DEFINE_SETTER2(variableType1, variableName1,default_value1,variableType2, variableName2,default_value2,funcName, className) \
     public:\
    virtual className& set##funcName(variableType1 value1,variableType2 value2) { \
        this->variableName1 = value1; \
        this->variableName2 = value2; \
        return *this;\
    }\
     protected:\
    variableType1 variableName1 = default_value1;\
    variableType2 variableName2 = default_value2;\


#endif // _TIGER_PARAMETERS_H_