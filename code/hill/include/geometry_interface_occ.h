#pragma once
#ifndef CREATE_GEOMETRY_H_
#define CREATE_GEOMETRY_H_
#include "geometry_data_struct.h"

namespace TiGER{
        /**
         * @brief 读取几何到内存中
         * 
         * @param[in] filename 读入文件名，支持 stp, igs, step, iges, sat, brep, x_t ...
         **/
        void readModel_OCC(const std::string& filename,TiGER::Geometry& geometry);

        /**
        * @brief 生成STL（离散背景网格）
        * 
        * @param[out] tris 生成的STL三角形拓扑
        * @param[out] points 生成的STL点坐标
        **/
        void generateStl_OCC(
            TiGER::Geometry& geometry,
            std::vector<std::array<int,3>>& tris,
            std::vector<std::array<double,3>>& points);
}
#endif