#include <iostream>
#include <set>
#include <map>
#include <array>
#include <vector>
#include <string>
#include <functional>

#include "./tetrahedral_mesh_parameters.h"
#include "./mesh_data_struct.h"
#include "./definitions.h"

namespace TiGER
{
	enum AttributeType
	{
		point_id = 0,
		sizing_value,
		surface_id,
		color,
		point_normal,
		facet_normal
	};

	/**
	 * @brief 体网格生成模块命名空间
	 */
	namespace tetrahedral_mesh
	{
		/**
		 * @brief 约束体网格边界恢复
		 *
		 * @param surface_mesh 输入的表面网格网格拓扑
		 * @param args 输入的控制参数
		 * @param volume_mesh 输出的体网格拓扑
		 */
		Tiger_API void constrainDelaunay(
			const SurfaceMesh& surface_mesh,
			const TetrahedraParameters& args,
			VolumeMesh& volume_mesh,
			const std::function<double(const std::array<double, 3>)>& func
		) ;

		/**
		 * @brief 带盒子的保形四面体边界恢复
		 *
		 * @param surface_mesh 输入的表面网格网格拓扑
		 * @param args 输入的控制参数
		 * @param volume_mesh 输出的体网格拓扑
		 */
		Tiger_API void boxConformingDelaunay(
			const SurfaceMesh &surface_mesh,
			const TetrahedraParameters &args,
			VolumeMesh &volume_mesh);

		/**
		 * @brief 保形体网格边界恢复
		 *
		 * @param surface_mesh 输入的表面网格网格拓扑
		 * @param args 输入的控制参数
		 * @param volume_mesh 输出的体网格拓扑
		 * @param surface_mesh_out 输出的表面网格拓扑
		 */
		Tiger_API void conformingDelaunay(
			const SurfaceMesh &surface_mesh,
			const TetrahedraParameters &args,
			VolumeMesh &volume_mesh,
			SurfaceMesh &surface_mesh_out);

		/**
		 * @brief 各向异性的体网格优化
		 *
		 * @param volume_mesh_in 输入的体网格拓扑
		 * @param args 输入的控制参数
		 * @param volume_mesh_out 输出的体网格拓扑
		 * @param func 输入的张量尺寸场
		 */
		Tiger_API void tetOptimization(
			const VolumeMesh &volume_mesh_in,
			const TetrahedraParameters &args,
			VolumeMesh &volume_mesh_out,
			std::function<std::array<double, 9>(std::array<double, 3>)> &func);
	}
}
