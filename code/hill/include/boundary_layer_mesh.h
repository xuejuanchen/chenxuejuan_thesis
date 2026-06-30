#pragma once
#include "./mesh_data_struct.h"
#include "./boundary_layer_mesh_parameters.h"
#include "./tetrahedral_mesh_parameters.h"
#include "./definitions.h"
#include "./geometry_data_struct.h"

#include <vector>
#include <array>
namespace TiGER{
    namespace boundary_layer{
        
        /**
         * @brief 生成三棱柱网格，同时也生成少量的四面体、金字塔等过渡单元
         * 
         * @param[in] boundary_mesh 输入面网格，包括点坐标 surface_mesh.coords,
         *  拓扑连接surface_mesh.tris,
         *  面id信息surface_mesh.regions
         * @param[in] surfaces 输入的对称面几何面，第N个面对应args中提到的第N个对称面
         * @param[in] boundary_conditions 边界条件，默认为WALL
         * @param[in] args 输入参数
         * @param[out] vol_mesh 输出体网格，包括三棱柱网格prisms，少量四面体tetras和金字塔pyramids网格
         * @param[out] out_boundary_mesh 输出修改的面网格
         */
        Tiger_API void generatePrismaticMesh(
            const SurfaceMesh& boundary_mesh,
            const std::vector<std::shared_ptr<TiGER::GeometrySurface>>& surfaces,
            const BoundaryLayerParameters& args,
            VolumeMesh& vol_mesh,
            SurfaceMesh& out_boundary_mesh,
			SurfaceMesh& tris_boundary_mesh
        );



        /**
         * @brief 生成混合网格，包括三棱柱网格、四面体、金字塔等过渡单元
         * 
         * @param[in] boundary_mesh 输入面网格，包括点坐标 surface_mesh.coords,
         *  拓扑连接surface_mesh.tris,
         *  面id信息surface_mesh.regions
         * @param[in] surfaces 输入的对称面几何面，第N个面对应args中提到的第N个对称面
         * @param[in] boundary_conditions 边界条件，默认为WALL
         * @param[in] args 输入参数
         * @param[out] vol_mesh 输出体网格，包括三棱柱网格prisms，少量四面体tetras和金字塔pyramids网格
         * @param[out] out_boundary_mesh 输出修改的面网格
         */
        Tiger_API void generateHybridMesh(
            const SurfaceMesh& boundary_mesh,
            const std::vector<std::shared_ptr<TiGER::GeometrySurface>>& surfaces,
            const BoundaryLayerParameters& args,
            const TetrahedraParameters& tet_args,
            VolumeMesh& vol_mesh,
            SurfaceMesh& out_boundary_mesh
        );


        
        /**
         * @brief 生成六面体为主网格,少量四面体tetras和金字塔pyramids网格
         * 
         * @param[in] boundary_mesh 输入面网格，包括点坐标 surface_mesh.coords,
         *  拓扑连接surface_mesh.tris,
         *  面id信息surface_mesh.regions
         * @param[in] surfaces 输入的对称面几何面，第N个面对应args中提到的第N个对称面
         * @param[in] boundary_conditions 边界条件，默认为WALL
         * @param[in] args 输入参数
         * @param[out] vol_mesh 输出体网格，包括六面体网格hexs，少量四面体tetras和金字塔pyramids网格
         * @param[out] out_boundary_mesh 输出修改的面网格
         */
        void generateHexMesh(
            const SurfaceMesh& boundary_mesh,
            const std::vector<BoundaryCondition>& boundary_conditions,
            const std::vector<GeometrySurface>& surfaces,
            const BoundaryLayerParameters& args,
            VolumeMesh& vol_mesh,
            SurfaceMesh& out_boundary_mesh
        );
    }
};