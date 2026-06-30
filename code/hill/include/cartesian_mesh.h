#pragma once
#include "./mesh_data_struct.h"
#include "./cartesian_mesh_parameters.h"

namespace TiGER
{
    namespace Cartesian{
    /**
     * @brief 生成笛卡尔网格
     * 
     * @param surf_mesh 输入的表面三角形网格
     * @param args 输入的控制参数
     * @param vol_mesh 输出的笛卡尔网格
     */
    void generateCartesianMesh(
        const SurfaceMesh& surf_mesh,
        const CartesianParameters& args,
        VolumeMesh& vol_mesh
    );
    }
} // namespace TiGER
