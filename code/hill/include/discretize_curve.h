#include "geometry_data_struct.h"
#include "./sizing_field_interface.h"
#include "./mesh_data_struct.h"
#include "./definitions.h"
#include "./discretize_curve_parameters.h"

#include <memory>
#include <string>
/**
 * @brief 曲线离散函数集 
 *  
 */
namespace TiGER{
namespace discretize_curve{


    Tiger_API void setLineMesh(std::shared_ptr<TiGER::GeometryCurve>curve,const std::vector<std::array<double,3>>& points);

    /**
     * @brief 根据尺寸场离散虚曲线 
     * 
     * @param curves 一组收尾相连的几何曲线 
     * @param sizefield 尺寸场 
     * @param discrete_curve 离散曲线结果 
     */

    Tiger_API void discreteCurve(

        std::shared_ptr<TiGER::GeometryCurve>curve,
        const SizingFunction& sizefield, 
        const TiGER::CurveParameters& args,
        TiGER::LineMesh& segments);


    /**
     * @brief 以Dimension方式离散一组曲线
     * 类型包括 
     * 
     * @param curves 一组曲线
     * @param args Dimension参数
     * @param segements 离散结果
     */
    Tiger_API void discreteCurveDimension(
        std::vector<std::shared_ptr<TiGER::GeometryCurve>>& curves, 
        const TiGER::CurveParametersDimension& args,
        std::vector<TiGER::LineMesh>& segements
    );
    /**
     * @brief 以Shape等特殊方式离散一组虚曲线
     * 类型包括 
     * 
     * @param curves 一组虚曲线
     * @param sizefield 尺寸场
     * @param discrete_curve 离散结果
     */
    Tiger_API void discreteCurvesTanh(
        const std::vector<std::shared_ptr<GeometryCurve>>& curves, 
        const TiGER::CurveParametersTanh& args, 
        std::vector<TiGER::LineMesh>& segements);

    /**
     * @brief 以Shape等特殊方式离散一组虚曲线
     * 类型包括 
     * 
     * @param curves 一组虚曲线
     * @param sizefield 尺寸场
     * @param discrete_curve 离散结果
     */
    Tiger_API void discreteCurvesGeometric(
        const std::vector<std::shared_ptr<GeometryCurve>>& curves, 
        const TiGER::CurveParametersGeometric& args, 
        std::vector<TiGER::LineMesh>& segements);
    /**
     * @brief 以Growth等特殊方式离散一组虚曲线
     * 类型包括 
     * 
     * @param curves 一组虚曲线
     * @param sizefield 尺寸场
     * @param discrete_curve 离散结果
     */
    Tiger_API void discreteCurvesShape(
        const std::vector<TiGER::GeometryCurve>& curves, 
        const TiGER::CurveParametersGrowth& args, 
        std::vector<TiGER::LineMesh>& segements);


    /**
     * @brief 以Automatic等特殊方式离散一组虚曲线
     * 类型包括 
     * 
     * @param curves 一组虚曲线
     * @param sizefield 尺寸场
     * @param discrete_curve 离散结果
     */
    Tiger_API void discreteCurvesAutomatic(
        const std::vector<TiGER::GeometryCurve>& curves, 
        const TiGER::CurveParametersAutomatic& args, 
        std::vector<TiGER::LineMesh>& segements);


} // discretize_curve
} // TiGER