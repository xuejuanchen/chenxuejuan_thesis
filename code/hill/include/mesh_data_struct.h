#pragma once
#ifndef _MESH_DATA_STRUCT_H_
#define _MESH_DATA_STRUCT_H_
#include <vector>
#include <array>
#include <map>
#include <functional>
#include <unordered_set>

namespace TiGER
{
     struct LineMesh
    {
        std::vector<std::array<double, 3>> coord;           //  点坐标数组 
        std::vector<std::array<int,2> > segments;            // 线段信息 
        std::vector<int> regions;                           // 区域信息面ID数组 
        std::vector<std::array<double, 3>> point_normal;   // 点法线数组 
    };

    
    enum BoundaryCondition {
        WALL = 0,
        SYMMETRY,
        FARFIELD,
        INLET,
        OUTLET,
        PERIODIC_SYMMETRY,
		ADJACENT_GRID,
		MATCH_NO_PUSH
    };


    /**
     * @brief 表示三维线网格的数据结构，在点位置上按顺序排列 
     */

   

    /**
     * @brief 表示三维表面网格的数据结构
     */
    struct SurfaceMesh
    {
        // 信息
        std::vector<std::array<double, 3>> coords;           // 点坐标数组（必须） 
        std::vector<std::array<double, 2>> uv;              // UV坐标数组 
        std::vector<std::array<int, 2>> edges;               // 边数组，记录边
        std::vector<std::array<int, 3>> tris;               // 三角形面数组,默认顺序优先三角形网格
        std::vector<std::array<int, 4>> quads;              // 四边形面数组，默认三角形网格存储完成后存储四边形网格 
        std::vector<int> regions;                           // 区域信息/面ID信息数组,维度为tris.size()+quads.size()
        std::vector<int> uv_regions;                        // UV区域信息数组 
        std::vector<std::array<double, 3>> point_normal;   // 点法线数组 
        std::vector<std::array<double, 3>> facet_normal;   // 面法线数组 
        std::vector<int> attribute_int;                     // 整型属性数组，通常为面id 
        std::vector<double> point_attribute_double;                 // 浮点型属性数组 
        std::map<int, BoundaryCondition>  bc;                //边界条件映射

        // 拓扑
        std::vector<std::unordered_set<int>> v_conn_tris;
        std::vector<std::unordered_set<int>> v_conn_points;
        
    };

    /**
     * @brief 表示三维体网格的数据结构。
     */
    struct VolumeMesh
    {
        std::vector<std::array<double, 3>> coords;          // 点坐标数组（必须）
        std::vector<std::array<int, 4>> tetras;             // 四面体单元数组
        std::vector<std::array<int, 6>> prisms;             // 棱柱单元数组
        std::vector<std::array<int, 5>> pyramids;           // 金字塔单元数组
        std::vector<std::vector<int>> polygons;             // 其他类型单元数组
        std::vector<int> regions;                           // 区域信息数组

     };

    /**
     * @brief 表示并行三维体网格的数据结构。
     */
    struct ParallelVolumeMesh
    {
        std::vector<std::array<double, 3>> coords;          // 点坐标数组
        std::vector<std::array<int, 4>> tetras;             // 四面体单元数组
        std::vector<std::array<int, 6>> prisms;             // 棱柱单元数组
        std::vector<std::array<int, 5>> pyramids;           // 金字塔单元数组
        std::vector<long long> local_to_global;             // 局部到全局索引映射数组
        std::vector<std::vector<int>> polygons;             // 其他类型单元数组

     };
}

#endif 