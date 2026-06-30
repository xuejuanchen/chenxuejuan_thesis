#pragma once
#ifndef _DISCRETE_GEOMETRY_REPAIR_PARAMETERS_H
#define _DISCRETE_GEOMETRY_REPAIR_PARAMETERS_H
#include "./parameters.h"
#include "./mesh_quality.h"
namespace TiGER{
    struct RepairParameters : public APIPara
    {
        public:
            DEFINE_SETTER(int, operator, RepairParameters);
            DEFINE_SETTER(std::vector<double>, tolerance, RepairParameters);
            DEFINE_SETTER(SurfMeshQualityType, type, RepairParameters);

        private:
            int operator;  // 运算符 0小于  1 大于  2 介于  3 不介于   默认小于
            std::vector<double> tolerance;   // 阈值  范围区间时容差有多个  
            SurfMeshQualityType type;  // 质量类型  面积  ...

            int fill_mode;  // 填充模式  0混合  1仅限开环  2仅限闭合  默认2
            bool preserve_local;  // true 保持局部曲率  默认 false
            int openloop_option;  // 1 分别为每个开环填充   2 在开环对之间填充  默认1
            int fill_option;  // 1 分别填充每个孔  2 同时填充所有孔

            int surface_option;  // 填充多边形块选项  0 默认 将各块面分配给点击的第一个部件表面  1 指定 将各块面分配给指定部件表面
            
        /* data */
    };
     
}

#endif // _DISCRETE_GEOMETRY_REPAIR_PARAMETERS_H