#pragma once

// #include "mesh_data_struct.h"
// #include "definitions.h"
// #include <functional>
// #include <vector>
#include "mesh_repair.h"
#include "binary_tree.hpp"

namespace TiGER {
namespace repair {
// 引入 std 命名空间
// using namespace std;

int find(std::vector<int>& belong, const int x) {
  if (belong[x] != x) {
    belong[x] = find(belong, belong[x]);  // 路径压缩
  }
  return x;
}
void unionSet(std::vector<int>& belong, const int a, const int b) {
  int root_a = find(belong, a);
  int root_b = find(belong, b);
  if (root_a != root_b) {
    belong[root_b] = root_a;
  }
}

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
int detectDuplicatePoints(const std::vector<std::array<double, 3>>& points_in,
                          const double& eps, std::vector<int>& bcj_pre) {
  int count = 0;
  bcj_pre.resize(points_in.size());
  std::vector<int> belong(points_in.size());
  for (int i = 0; i < points_in.size(); i++) {
    belong[i] = i;  // 初始化 每个节点都各自为一组
  }

  // 暴力
  // for(int i=0;i<points_in.size();i++){
  //     Eigen::Vector3d
  //     v1(points_in[i][0],points_in[i][1],points_in[i][2]); for(int
  //     j=i+1;j<points_in.size();j++){
  //         Eigen::Vector3d
  //         v2(points_in[j][0],points_in[j][1],points_in[j][2]); double
  //         dist = (v1-v2).norm(); if(dist <= eps){  // 重复点
  //             unionSet(bcj_pre,i,j);  // 合并到前一个节点的集合
  //             count++;
  //         }
  //     }
  // }

  // 八叉树
  TiGER::common::BinaryTree<Eigen::Matrix3d> envelop_tree_;
  Eigen::MatrixXd V(points_in.size(), 3);
  for (int i = 0; i < points_in.size(); i++) {
    V.row(i) =
        Eigen::RowVector3d(points_in[i][0], points_in[i][1], points_in[i][2]);
  }
  envelop_tree_.init(V);
  for (int i = 0; i < points_in.size(); i++) {
    Eigen::Vector3d v1(points_in[i][0], points_in[i][1], points_in[i][2]);
    std::vector<size_t> result;
    envelop_tree_.query(V.row(i), eps, result);
    for (int j = 0; j < result.size();
         j++) {  // result 里面存的的点box与（容差范围box）相交
      // 容差范围内的点
      Eigen::Vector3d v2(points_in[result[j]][0], points_in[result[j]][1],
                         points_in[result[j]][2]);
      double dist = (v1 - v2).norm();
      if (dist <= eps) {
        if (belong[i] == i && result[j] > i &&
            belong[result[j]] == result[j]) {  // 防止重复操作
          // std::cout<<i<<" "<<result[j]<<" "<<dist<<endl;
          unionSet(belong, i, result[j]);  // 合并到前一个节点的集合
          count++;
        }
      }
    }
  }
  for (int i = 0; i < points_in.size(); i++) {
    if (belong[i] == i) {
      bcj_pre[i] = -1;
    } else {
      bcj_pre[i] = belong[i];
    }
  }
  for (int i = 0; i < points_in.size(); i++) {
    if (belong[i] == i) {
      bcj_pre[i] = -1;
    } else {
      bcj_pre[i] = belong[i];
    }
  }
  return count;
}
}  // namespace repair
}

