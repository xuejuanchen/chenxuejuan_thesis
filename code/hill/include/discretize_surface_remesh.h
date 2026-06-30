
#include "geometry_data_struct.h"
#include "./mesh_data_struct.h"
#include "discretize_surface_remesh_parameters.h"
#include "definitions.h"
#include <functional>

#include <string>

namespace TiGER{
    namespace discretize_surface_remesh{



        // /**
        //  * @brief 保边界的单张曲面重网格化
        //  * 
        //  * @param[in] surface 几何面 
        //  * @param[in] surfmesh_in 输入面网格
        //  * @param[in] sizefield 输入尺寸场
        //  * @param[in] args 输入参数字符串
        //  * @param[out] surfmesh_out 重网格化的面网格
        //  */
        // void triangleRemeshOnSurface(
        //     const GeometrySurface& surface,
        //     const SurfaceMesh& surfmesh_in,
        //     const SizingField& sizefield,
        //     const std::string& args,
        //     SurfaceMesh& surfmesh_out
        // );
        
        // /**
        //  * @brief 多个曲面的重网格化 
        //  * 
        //  * @param[in] surface 几何面 
        //  * @param[in] surfmesh_in 输入面网格
        //  * @param[in] sizefield 输入尺寸场
        //  * @param[in] args 输入参数字符串
        //  * @param[out] surfmesh_out 重网格化的面网格
        //  */
        // void triangleRemeshOnSurfaces(
        //     const std::vector<GeometrySurface>& surface,
        //     const SurfaceMesh& surfmesh_in,
        //     const SizingField& sizefield,
        //     const std::string& args,
        //     SurfaceMesh& surfmesh_out
        // );


        /**
         * @brief 重新网格化接口
         * 
         * @param surfmesh_in  输入的曲面网格
         * @param constrained_edge 硬约束边
         * @param conformaing_edge 软约束边
         * @param args 控制参数
         * @param size_function 尺寸场
         * @param surfmesh_out 输出的网格优化后的曲面网格
         * @param constrained_edge_out 输出的硬约束边
         * @return Tiger_API 
         */
        Tiger_API void triangleRemesh(
            const SurfaceMesh &surfmesh_in,
            const std::vector<std::vector<int>> &constrained_edge,
            const std::vector<std::vector<int>> &conforming_edge,
            const RemeshParameters &args,
            std::function<double(double, double, double)> &size_function,
            SurfaceMesh &surfmesh_out,
            std::vector<std::vector<int>> &constrained_edge_out
            );

        /**
         * @brief 重新网格化接口
         * 
         * @param surfmesh_in  输入的曲面网格
         * @param surfaces_in  输入的连续几何
         * @param constrained_edge 硬约束边
         * @param conformaing_edge 软约束边
         * @param args 控制参数
         * @param size_function 尺寸场
         * @param surfmesh_out 输出的网格优化后的曲面网格
         * @param constrained_edge_out 输出的硬约束边
         * @return Tiger_API 
         */
        Tiger_API void triangleRemesh(
            const SurfaceMesh &surfmesh_in,
            const std::vector<GeometrySurface>& surfaces_in,
            const std::vector<std::vector<int>> &constrained_edge,
            const std::vector<std::vector<int>> &conforming_edge,
            const RemeshParameters &args,
            std::function<double(double, double, double)> &size_function,
            SurfaceMesh &surfmesh_out,
            std::vector<std::vector<int>> &constrained_edge_out
        );

        // /**
        //  * @brief 保边界的单张曲面重网格化
        //  * 
        //  * @param[in] surface 几何面 
        //  * @param[in] surfmesh_in 输入面网格
        //  * @param[in] sizefield 输入尺寸场
        //  * @param[in] args 输入参数字符串
        //  * @param[out] surfmesh_out 重网格化的面网格
        //  */
        // void quadRemeshOnSurface(
        //     const GeometrySurface& surface,
        //     const SurfaceMesh& surfmesh_in,
        //     const SizingField& sizefield,
        //     const std::string& args,
        //     SurfaceMesh& surfmesh_out
        // );
        
        // /**
        //  * @brief 多个曲面的重网格化 
        //  * 
        //  * @param[in] surface 几何面 
        //  * @param[in] surfmesh_in 输入面网格
        //  * @param[in] sizefield 输入尺寸场
        //  * @param[in] args 输入参数字符串
        //  * @param[out] surfmesh_out 重网格化的面网格
        //  */
        // void quadRemeshOnSurfaces(
        //     const std::vector<GeometrySurface>& surface,
        //     const SurfaceMesh& surfmesh_in,
        //     const SizingField& sizefield,
        //     const std::string& args,
        //     SurfaceMesh& surfmesh_out
        // );
    }
}