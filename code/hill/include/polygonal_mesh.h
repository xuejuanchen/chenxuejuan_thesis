#include <iostream>
#include <set>
#include <map>
#include <array>
namespace TiGER{
    /**
     * @brief 生成多面体网格
     * 
     * @param[in] vol_in 输入的四面体网格
     * @param[in] args 输入的控制参数
     * @param[out] vol_out 输出的多面体网格
     */
    void generatePolygonalMesh(
      const VolumeMesh& vol_in,
      const std::string& args,
      VolumeMesh& vol_out,
    );
}