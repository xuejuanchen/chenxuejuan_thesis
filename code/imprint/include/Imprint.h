#ifndef PRETREATED_H
#define PRETREATED_H
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <set>
#include <iostream>
#include <vector>
#include <Eigen/Dense>
#include "interface.h"
#include "Parameterization.h"
#include "binary_tree.hpp"
struct Imprint
{
	/* 频选单元四面体结构 */
	Eigen::MatrixXi tetra_topo; // 四面体拓扑
	std::vector<Eigen::RowVector3d> tetra_vertices; // 四面体顶点
	std::vector<int> tetra_id; // 每个三角单元顶点所属的四面体id dim = dim(u_mesh.Vertex)
	std::vector<int> bottom_v_id; // 频选单元底面上的点的id
	std::vector<int> top_v_id; // 频选单元顶面上的点的id
	std::vector<Eigen::RowVector4d> barycentric_coords; // 每个三角单元顶点在其所属四面体id中的重心坐标 dim = dim(u_mesh.Vertex)
	std::vector<int> pointFaces; // 每个点所属的边界面，0内部，1前，2后，3左，4右，5上，6下
	std::vector<int> symmetricPointIds; // 每个点对称的点
	std::vector<int> left, right, front, back;
	void UnitImprint(Mesh& grid, Mesh& u_mesh, Mesh& output);
	void UnitMeshPre(Mesh& mesh);
	void BottomProject(Mesh& mesh, Parameterization& param);
	void TopProject(Mesh& mesh, Mesh& grid, Parameterization& param);
	void TopProject(Mesh& mesh, Mesh& grid, Mesh& u_mesh, Parameterization& param);
	void RemoveDuplicate(Mesh& mesh, double eps);
};

#endif