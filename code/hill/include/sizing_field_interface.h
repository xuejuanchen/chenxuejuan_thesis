#pragma once
#include <vector>
#include <array>

#include "./geometry_data_struct.h"

#include "sizing_field_parameters.h"

namespace TiGER{

    /**
 * @brief 内部实现
 *
 */
    typedef std::function<double(const double&,const double&,const double&)> SizingFunction;
/**
 * @brief 内部实现
 * 
 */
struct SizingManager{
    public:
    std::vector<SizingFunction> sf;
};

} // TiGER

/**
 * @brief TODO 参数类，写进去
 * 
 * 
 */
namespace TiGER{
    namespace size_field{
          /**
    * @brief 生成（离散背景网格）
    * 
    * @param[in] hmin 最小尺寸
    * @param[in] hmax 最大尺寸
    * @param[in] beta 尺寸过渡因子
    * @param[in] theta 曲率自适应因子
    * @param[in] proximity 邻近因子
    * @param[out] sf 尺寸管理对象
    */
    void setSizeField(
      const SurfaceMesh& SurfaceMesh,
      const BackGroundParameters& args,
      SizingManager& sf
      ) ;
    }


    /**
    * @brief 生成STL（离散背景网格）
    * 
    * @param[in] hmin 最小尺寸
    * @param[in] hmax 最大尺寸
    * @param[in] beta 尺寸过渡因子
    * @param[in] theta 曲率自适应因子
    * @param[in] proximity 邻近因子
    * @param[out] sf 尺寸管理对象
    */
    void setGeometrySizeField(
      const Geometry& geometry,
      const SourceParameters& args,
      SizingManager& sf
      ) ;
    

    /**
     * @brief 点源
     * 
     * @param[in] xyz 点位置
     * @param[in] inner_radius 内径
     * @param[in] outter_radius 外径
     * @param[in] sizing_value 尺寸值
     * @param[in] manager 尺寸管理对象
     */
    void addPointSource(const std::array<double,3>& xyz,
      const PointSourceParameters& args,
        SizingManager& manager);

        /**
     * @brief 线源
     * 
     * @param[in] xyz 点位置
     * @param[in] manager 尺寸管理对象
     */

    void addLineSource(const std::array<std::array<double,3>,2>& xyz,
      const SourceParameters& args,
        SizingManager& manager);

           /**
     * @brief 三角形源
     * 
     * @param[in] xyz 点位置
     * @param[in] manager 尺寸管理对象
     */

    void addTriangleSource(const std::array<std::array<double,3>,3>& xyz,
      const SourceParameters& args,
        SizingManager& manager);

    /**
     * @brief 多边形源
     * 
     * @param[in] xyz 点位置
     * @param[in] inner_radius 内径
     * @param[in] outter_radius 外径
     * @param[in] sizing_value 尺寸值
     * @param[in] manager 尺寸管理对象
     */

    void addPolygonSource(const std::vector<std::array<double,3>>& xyz,
        const std::vector<double>& inner_radius,
        const std::vector<double>& outter_radius,
        const double& sizing_value,
        SizingManager& manager);
        
 
     /**
     * @brief 椭球源
     * 
     * @param[in] xyz1 1号焦点位置
     * @param[in] xyz2 2号焦点位置
     * @param[in] inner_radius 内径
     * @param[in] outter_radius 外径
     * @param[in] sizing_value 尺寸值
     * @param[in] manager 尺寸管理对象
     */

    void addEllipsoidSource(const std::array<double,3>& xyz1,
    const std::array<double,3>& xyz2,
        const double& inner_radius,
        const double& outter_radius,
        const double& sizing_value,
        SizingManager& manager);


     /**
     * @brief 圆台源
     * 
     * @param[in] upper_xyz 上表面圆心位置
     * @param[in] lower_xyz 下表面圆心位置
     * @param[in] upper_radius 上表面圆半径
     * @param[in] lower_radius 下表面圆半径
     * @param[in] outter_radius 外径
     * @param[in] sizing_value 尺寸值
     * @param[in] manager 尺寸管理对象
     */

    void addFrustumSource(const std::array<double,3>& upper_xyz,
        const std::array<double,3>& lower_xyz,
        const double& upper_radius,
        const double& lower_radius,
        const double& outter_radius,
        const double& sizing_value,
        SizingManager& manager);

     /**
     * @brief 立方体源
     * 
     * @param[in] points 立方体8个点位置
     * @param[in] outter_radius 外径
     * @param[in] sizing_value 尺寸值
     * @param[in] manager 尺寸管理对象
     */

    void addCubicSource(
        const std::array<std::array<double,3>,8>& points,
        const double& outter_radius,
        const double& sizing_value,
        SizingManager& manager);
        
     /**
     * @brief 增加用户自定义尺寸场
     * 
     * @param[in] function 用户自定义尺寸场
     * @param[in] manager 尺寸管理对象
     */

    void addUserDefineSize(
        const std::function<std::array<double,9>(std::array<double,3>)> function,
        SizingManager& manager);
 
 
    /**
     * @brief 查询某点尺寸值
     * 
     * @param[in] xyz 点坐标
     * @param[in] manager 尺寸管理对象
     * @return 该点尺寸值
     */

    double getSizeValue(const std::array<double,3>& xyz,SizingManager& manager);


    /**
     * @brief 设置给定点、给定曲线、给定曲面的尺寸值
     * 
     * @param[in] attribute_name 特征名字，可定义为"surface_id","curve_id"等
     * @param[in] attribute_index 特征的索引
     * @param[in] sizing_value 尺寸值
     * @param[in] manager 尺寸管理对象
     */

    void setAttributeSize(
        const std::string& attribute_name,
        const int& attribute_index,
        const double& sizing_value,SizingManager& manager);
    
}