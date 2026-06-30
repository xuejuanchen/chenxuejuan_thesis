#pragma once

#include <vector>
#include <array>

#include "mesh_data_struct.h"
#include "definitions.h"
#include "autogrid_parameters.h"
namespace TiGER
{
    
namespace discrete_geometry_repair{
    /**
     * @brief 修复脏离散三角形网格，得到水密的无自相交的面网格
     * 
     * @param[in] surf_in 输入三角形表面网格
     * @param[in] local_eps 输入局部容差
     * @param[in] args 输入字符串控制参数
     * @param[out] surf_out 输出三角形表面网格
     */
Tiger_API void repairDiscreteModel(const SurfaceMesh& surf_in,
                                   const AutogridParameters& args,
                                   SurfaceMesh& surf_out);
}
} // namespace TiGER
