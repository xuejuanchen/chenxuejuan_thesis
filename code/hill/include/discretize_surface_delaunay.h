#include "./mesh_data_struct.h"
#include "./definitions.h"
#include "./discretize_surface_delaunay_parameters.h"
#include <string>
#include <functional>
namespace TiGER
{
	namespace discretize_surface_delaunay
	{
		/**
		 * @brief 三角化一个环
		 *
		 * @param[in] inLoop 输入的环
		 * @param[in] args 输入参数字符串
		 * @param[in] func 输入尺寸场
		 * @param[out] surfmesh_out 重网格化的面网格
		 */
		Tiger_API void triangleLoop(
			const LineMesh& Loop_in,
			const TriangleDealunay& args,
			std::function<double(const std::array<double, 3>)>& func,
			SurfaceMesh& surfmesh_out);
	}
}