#pragma once
#ifndef _DISCREATE_GEOMETRY_DATA_STRUCT_H_
#define _DISCREATE_GEOMETRY_DATA_STRUCT_H_
#include <array>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace TiGER {

/**
 * @brief 抽象几何曲线。
 */
struct DiscreteGeometryCurve {
  /**
   * @brief 函数对象，用于获取在指定U值处，曲线在U点的坐标值。
   *
   * @param[in] u 输入的UV参数空间中的U值。
   * @return std::array<double, 3> 曲线在U点的坐标值，为一个三维向量。
   **/
  std::function<std::array<double, 3>(const double &)> d0Function;

  /**
   * @brief 函数对象，用于将3D空间中的一个点投影到曲线上，并返回其对应的U值。
   *
   * @param[in] xyz 输入的3D空间中的点坐标。
   * @param[in] eps 投影操作允许的最大误差阈值。
   * @param[in] hint 提供的初值提示，有助于提高投影的计算效率和成功率。
   * @param[out] res_u 投影得到的点在UV参数空间中的U值。
   **/
  std::function<void(const std::array<double, 3> &, const double &,
                     const double &, double &)>
      projectFunction;

  /**
   * @brief 函数对象，用于根据曲线上的弧长值，找到对应的3D空间中的点坐标和U值。
   *
   * @param[in] arc_length 输入的曲线上的弧长值。
   * @param[out] xyz 根据弧长值计算得到的3D空间中的点坐标。
   * @param[out] u 对应的UV参数空间中的U值。
   **/
  std::function<void(const double &, std::array<double, 3> &, double &)>
      arcToCoordinateFunction;

  /**
   * @brief 函数对象，获取曲线总长
   **/
  std::function<double> getTotalLenthFunction;

  std::vector<int> curve_vertex_ids;
};

struct DiscreteGeometrySurface {
  /**
   * @brief 函数对象，根据UV参数空间中的坐标获取对应的三维空间中的坐标。
   *
   * @param uv UV参数平面坐标，{u, v}。
   * @return std::array<double, 3> 三维空间中的坐标，{x, y, z}。
   */
  std::function<std::array<double, 3>(std::array<double, 2>)> d0Function;

  /**
   * @brief 函数对象，计算UV参数空间中指定坐标处的U方向和V方向的导数。
   *
   * @param uv UV参数平面坐标。
   * @param du U方向的导数。
   * @param dv V方向的导数。
   */
  std::function<void(const std::array<double, 2>&, std::array<double, 3>&, std::array<double, 3>&)>
      d1Function;

  /**
   * @brief 函数对象，计算UV参数空间中指定坐标处的U方向二阶导数、V方向二阶导数和UV方向二阶导数。
   *
   * @param uv UV参数平面坐标。
   * @param duu U方向的二阶导数。
   * @param dvv V方向的二阶导数。
   * @param duv UV方向的二阶导数。
   */
  std::function<void(const std::array<double, 2>&, std::array<double, 3>&, std::array<double, 3>&, std::array<double, 3>&)>
      d2Function;

  /**
   * @brief 函数对象，将一个点投影到曲面上。
   *
   * @param point 要投影到曲面上的点。
   * @param tolerance 投影的容差值。
   * @param projectionResult 投影后的点坐标/提示值。
   * @return std::array<double, 3>& 投影后的点坐标的引用。
   */
  std::function<std::array<double, 3>&(const double&, std::vector<double>&)> projectFunction;

  std::vector<std::array<int, 3>> tris;  /// 三角形面ID
  std::vector<std::array<int, 4>> quads; /// 四边形面ID，排序在三角形面之后
};


/**
 * @brief 几何信息结构体。
 */
struct DiscreteGeometry {
  /** 点坐标 **/
  std::vector<std::array<double, 3>> coords;
  /** 几何信息 */
  std::vector<std::shared_ptr<DiscreteGeometryCurve>> curves;
  std::vector<std::shared_ptr<DiscreteGeometrySurface>> surfaces;

  /** 内约束 **/
  std::vector<std::shared_ptr<DiscreteGeometryCurve>> constrained_curve;
  std::vector<std::shared_ptr<DiscreteGeometryCurve>> conforming_curve;

  /** 拓扑信息 */
  std::vector<std::vector<int>> topo_surf_to_curves;  // 面到曲线的拓扑信息
  std::vector<std::vector<int>> topo_body_to_surfs;  // 体到面的拓扑信息

  /** 面名字信息 **/
  std::map<int, std::string> mp_index_name;

  typedef Point std::array<double, 3>;
  typedef PointIndex int;
  typedef Edge std::array<int,2>;
  typedef CurveIndex int;
  typedef TriangleIndex std::array<int,2>; // 面id - 三角片id
  typedef Triangle_Triangle std::array<TriangleIndex,2>;
  typedef SurfaceIndex int;
  typedef ConstrainedCurveIndex int;
  typedef ConformingCurveIndex int;
};

}  // namespace TiGER

#endif