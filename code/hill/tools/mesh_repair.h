#pragma once

#include "mesh_data_struct.h"
#include "definitions.h"
#include <functional>

namespace TiGER {
namespace repair {
/**
 * @brief
 *
 * @param points_in 输入的点
 * @param eps 去重的容差
 * @param bcj_pre 并查集
 *
 * points_in index =  0   1   2  3  4  5  6   7  /// 2，3，5，6 重复，4，7重复。
 * bcj_pre         = -1  -1  -1  2 -1  2  2   4  --> 并查集结果
 *
 * @return int 重复点的个数
 */
int detectDuplicatePoints(
    const std::vector<std::array<double, 3>>& points_in,
    const double& eps, 
    std::vector<int>& bcj_pre);

} // namespace repair
} // namespace TiGER

#include "mesh_repair.cpp"