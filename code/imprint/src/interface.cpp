#include "interface.h"
#include "Imprint.h"
#include "Parameterization.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <Eigen/Dense>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <vector>

/**
 * @brief BFS 搜索与指定拓扑 ID 相连的三角形 ID
 *
 * @param Topo 拓扑矩阵，每行表示一个三角形的顶点
 * @param start_id 起始顶点的 ID
 * @param max_depth 搜索的层数（深度限制）
 * @return std::unordered_set<int> 包含与起始 ID 相连的三角形 ID
 */
static std::unordered_set<int> bfsSearch(const Eigen::MatrixXi& Topo, int start_id, int max_depth) {
    // Step 1: 构建邻接表
    std::unordered_map<int, std::vector<int>> vertex_to_triangles;
    for (int tri_id = 0; tri_id < Topo.rows(); ++tri_id) {
        for (int vertex_id = 0; vertex_id < Topo.cols(); ++vertex_id) {
            int vertex = Topo(tri_id, vertex_id);
            vertex_to_triangles[vertex].push_back(tri_id);
        }
    }

    // Step 2: 初始化 BFS 所需的数据结构
    std::unordered_set<int> visited_triangles; // 已访问的三角形
    std::queue<std::pair<int, int>> queue;     // 队列，存储 <当前顶点 ID, 当前深度>

    // 从起始顶点开始搜索
    queue.push({ start_id, 0 });

    // Step 3: BFS 搜索
    while (!queue.empty()) {
        auto current = queue.front();
        queue.pop();

        int current_vertex = current.first;  // 当前顶点 ID
        int current_depth = current.second; // 当前深度

        // 如果超过最大深度，则停止
        if (current_depth > max_depth) {
            continue;
        }

        // 获取当前顶点相关联的所有三角形
        const auto& triangles = vertex_to_triangles[current_vertex];
        for (int tri_id : triangles) {
            // 如果三角形未访问过，则标记为已访问
            if (visited_triangles.insert(tri_id).second) {
                // 将与该三角形相连的顶点加入队列
                for (int i = 0; i < Topo.cols(); ++i) {
                    int next_vertex = Topo(tri_id, i);
                    if (next_vertex != current_vertex) {
                        queue.push({ next_vertex, current_depth + 1 });
                    }
                }
            }
        }
    }

    return visited_triangles;
}

// run接口的实现
IMPRINTOOL_API int run(ProjectIO& io){

    /**
     * 曲面处理模块：
     * 输入模型中心压印的三角面片id，以该三角形为中心，BFS搜索一定范围的三角面片，输入参数化
     * 把输入的曲面参数化，并增加h维度
     * 调用exe生成物理空间带厚度的曲面
     * 获得两个空间的映射关系（三棱柱到三棱柱）
    **/

    // BFS
    int center_id = 184; //449
    int max_depth = 5;
    std::unordered_set<int> selected_tri;
    selected_tri = bfsSearch(io.sf_input.Topo, center_id, max_depth);
    // 构造选中的部分mesh
    int global_id = 0;
    auto& sV = io.sf_select.Vertex;
    auto& sT = io.sf_select.Topo;
    //sV.resize(io.sf_input.Vertex.rows(), 3);
    sT.resize(selected_tri.size(), 3);
    for (auto id : selected_tri) {
        io.mp[global_id] = id;
        sT.row(global_id) = io.sf_input.Topo.row(id);
        global_id++;
    }
    std::unordered_map<int, int> pt_mp;
    std::vector<Eigen::RowVector3d> sele_pt;
    int ptID = 0;
    for (int i = 0; i < sT.rows(); i++) {
        for (int j = 0; j < sT.cols(); j++) {
            auto it = pt_mp.find(sT(i, j));
            if (it != pt_mp.end()) {
                sT(i, j) = it->second;
            }
            else {
                sele_pt.push_back(io.sf_input.Vertex.row(sT(i, j)));
                pt_mp[sT(i, j)] = ptID;
                sT(i, j) = ptID;
                ptID++;
            }
        }
    }
    sV.resize(sele_pt.size(), 3);
    for (int i = 0; i < sele_pt.size(); i++) {
        sV.row(i) = sele_pt[i];
    }
    Parameterization param;
    int res = param.arap_parameterization(io.sf_select);
    if (res != 0) std::cout << "arap failed!" << std::endl;
    param.io = io;

    /**
     * 部件处理模块
    **/
    Eigen::RowVector3d start_pt = io.sf_input.Vertex.row(center_id);
    Eigen::RowVector2d start_uv;
    res = param.xyz_to_uv(start_pt, start_uv);
    // 查找部件z坐标最小的点，与start_uv对齐
    double minz = 1000000;
    int minzID;
    for (int i = 0; i < io.unit_input.Vertex.rows(); i++) {
        if (io.unit_input.Vertex(i, 2) < minz) {
            minz = io.unit_input.Vertex(i, 2);
            minzID = i;
        }
    }

    /**
     * 铺设模块：
     * 确定部件放置位置
     * 获取每个点在参数空间的坐标
     * 通过映射关系获得在物理空间的点坐标
    **/
    Eigen::RowVector3d change(start_uv(0) - io.unit_input.Vertex(minzID, 0), start_uv(1) - io.unit_input.Vertex(minzID, 1), 0 - minz);
    for (int i = 0; i < io.unit_input.Vertex.rows(); i++) {
        io.unit_input.Vertex.row(i) += change;
        Eigen::RowVector3d temp = io.unit_input.Vertex.row(i);
        io.unit_input.Vertex.row(i) = param.uvh_to_xyz(temp);
    }

    /**
     * 压印模块：
     * 网格布尔运算，合并网格
     * 输出可视化
    **/

    /**
     * 将输入网格与unit合并
    **/
    io.output.Vertex.resize(io.sf_input.Vertex.rows() + io.unit_input.Vertex.rows(), 3);
    io.output.Vertex.block(0, 0, io.sf_input.Vertex.rows(), 3) = io.sf_input.Vertex;
    io.output.Vertex.block(io.sf_input.Vertex.rows(), 0, io.unit_input.Vertex.rows(), 3) = io.unit_input.Vertex;
    io.output.Topo.resize(io.sf_input.Topo.rows() + io.unit_input.Topo.rows(), 3);
    io.output.Topo.block(0, 0, io.sf_input.Topo.rows(), 3) = io.sf_input.Topo;
    for (int i = io.sf_input.Topo.rows(); i < io.output.Topo.rows(); i++) {
        io.output.Topo(i, 0) = io.unit_input.Topo(i - io.sf_input.Topo.rows(), 0) + io.sf_input.Vertex.rows();
        io.output.Topo(i, 1) = io.unit_input.Topo(i - io.sf_input.Topo.rows(), 1) + io.sf_input.Vertex.rows();
        io.output.Topo(i, 2) = io.unit_input.Topo(i - io.sf_input.Topo.rows(), 2) + io.sf_input.Vertex.rows();
    }
    return 0;
}

