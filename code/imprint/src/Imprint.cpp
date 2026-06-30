#include "Imprint.h"
#include "unordered_set"
/* 计算四面体体积 */
double TetrahedronVolume(const Eigen::RowVector3d& A, const Eigen::RowVector3d& B, const Eigen::RowVector3d& C, const Eigen::RowVector3d& D) {
	Eigen::Matrix4d mat;
	mat << A[0], A[1], A[2], 1.0,
		B[0], B[1], B[2], 1.0,
		C[0], C[1], C[2], 1.0,
		D[0], D[1], D[2], 1.0;

	return std::abs(mat.determinant()) / 6.0;
}

/* 判断点是否在四面体上 */
bool IsPointInTetrahedron(const Eigen::RowVector3d& A, const Eigen::RowVector3d& B, const Eigen::RowVector3d& C,
	const Eigen::RowVector3d& D, const Eigen::RowVector3d& P, Eigen::RowVector4d& barycentricCoords) {
	double V_ABCD = TetrahedronVolume(A, B, C, D);
	double V_PBCD = TetrahedronVolume(P, B, C, D);
	double V_PACD = TetrahedronVolume(A, P, C, D);
	double V_PABD = TetrahedronVolume(A, B, P, D);
	double V_PABC = TetrahedronVolume(A, B, C, P);

	if (std::abs(V_ABCD - (V_PBCD + V_PACD + V_PABD + V_PABC)) < 1e-6) {
		barycentricCoords[0] = V_PBCD / V_ABCD;
		barycentricCoords[1] = V_PACD / V_ABCD;
		barycentricCoords[2] = V_PABD / V_ABCD;
		barycentricCoords[3] = V_PABC / V_ABCD;
		return true;
	}

	return false;
}

struct CustomHash {
	std::size_t operator()(const Eigen::RowVector3d& vec) const {
		return std::hash<double>()(vec.x()) ^ std::hash<double>()(vec.y()) ^ std::hash<double>()(vec.z());
	}
};

void Imprint::UnitMeshPre(Mesh& mesh) {
	auto& Vertex = mesh.Vertex;

	/* 找到六面体的八个顶点 */
	Eigen::RowVector3d min_pt = Vertex.row(0);
	Eigen::RowVector3d max_pt = Vertex.row(0);
	for (int i = 0; i < Vertex.rows(); i++) {
		min_pt = min_pt.cwiseMin(Vertex.row(i));
		max_pt = max_pt.cwiseMax(Vertex.row(i));
	}
	tetra_vertices = {
		Eigen::RowVector3d(max_pt.x(), min_pt.y(), min_pt.z()),
		Eigen::RowVector3d(max_pt.x(), max_pt.y(), min_pt.z()),
		Eigen::RowVector3d(min_pt.x(), max_pt.y(), min_pt.z()),
		Eigen::RowVector3d(min_pt.x(), min_pt.y(), min_pt.z()),
		Eigen::RowVector3d(max_pt.x(), min_pt.y(), max_pt.z()),
		Eigen::RowVector3d(max_pt.x(), max_pt.y(), max_pt.z()),
		Eigen::RowVector3d(min_pt.x(), max_pt.y(), max_pt.z()),
		Eigen::RowVector3d(min_pt.x(), min_pt.y(), max_pt.z()),
	};

	bottom_v_id.clear();
	top_v_id.clear();
	for (int i = 0; i < Vertex.rows(); i++) {
		if (Vertex.row(i).z() == min_pt.z()) {
			bottom_v_id.push_back(i);
		}
		if (Vertex.row(i).z() == max_pt.z()) {
			top_v_id.push_back(i);
		}
	}

	std::unordered_map<Eigen::RowVector3d, int, CustomHash> input_pt;
	for (int i = 0; i < Vertex.rows(); i++) {
		input_pt[Vertex.row(i)] = i;
	}
	std::vector<int> vertices;
	for (const auto& point : tetra_vertices) {
		auto it = input_pt.find(point);
		if (it != input_pt.end()) {
			vertices.push_back(it->second);
		}
		else {
			std::cout << "Point " << point << " not found in the set." << std::endl;
		}
	}

	/* 构造四面体拓扑 */
	tetra_vertices.push_back((min_pt + max_pt) / 2);
	tetra_topo.resize(12, 4);
	tetra_topo.row(0) << 0, 1, 4, 8, tetra_topo.row(1) << 1, 5, 4, 8; // 前面，1
	tetra_topo.row(2) << 3, 2, 7, 8, tetra_topo.row(3) << 2, 6, 7, 8; // 后面，2
	tetra_topo.row(4) << 1, 2, 5, 8, tetra_topo.row(5) << 2, 6, 5, 8; // 右面，4
	tetra_topo.row(6) << 0, 3, 4, 8, tetra_topo.row(7) << 3, 7, 4, 8; // 左面，3
	tetra_topo.row(8) << 0, 1, 3, 8, tetra_topo.row(9) << 1, 2, 3, 8; // 下面，6
	tetra_topo.row(10) << 4, 5, 7, 8, tetra_topo.row(11) << 5, 6, 7, 8; // 上面，5

	/* 获取每个点的重心坐标 */
	tetra_id.resize(Vertex.rows());
	barycentric_coords.resize(Vertex.rows());
	for (int i = 0; i < Vertex.rows(); i++) {
		Eigen::RowVector3d P = Vertex.row(i);
		bool found = false;

		for (int j = 0; j < tetra_topo.size(); j++) {
			Eigen::RowVector4i tet = tetra_topo.row(j);
			Eigen::RowVector3d A = tetra_vertices[tet(0)];
			Eigen::RowVector3d B = tetra_vertices[tet(1)];
			Eigen::RowVector3d C = tetra_vertices[tet(2)];
			Eigen::RowVector3d D = tetra_vertices[tet(3)];

			if (IsPointInTetrahedron(A, B, C, D, P, barycentric_coords[i])) {
				tetra_id[i] = j;
				found = true;
				break;
			}
		}
		if (!found)
			std::cout << "Query point " << i << " is not in any tetrahedron." << std::endl;
	}

	int numPoints = Vertex.rows();
	// 用于存储每个点属于哪个边界面，0表示内部，1-6表示具体的边界面
	pointFaces.resize(numPoints, 0);
	// 用于存储每个点的对称点ID
	symmetricPointIds.resize(numPoints, -1);

	for (int i = 0; i < numPoints; ++i) {
		Eigen::RowVector3d point = Vertex.row(i);

		// 判断点属于哪个面并计算对称点
		if (std::abs(point[0] - max_pt[0]) < 1e-5) {  // 前面（x = max_x）
			pointFaces[i] = 2;
			front.push_back(i);
			for (int j = 0; j < numPoints; ++j) {
				if (std::abs(Vertex.row(j)[0] - min_pt[0]) < 1e-5 && Vertex.row(j)[1] == point[1] && Vertex.row(j)[2] == point[2]) {
					symmetricPointIds[i] = j;
					break;
				}
			}
		}
		else if (std::abs(point[1] - min_pt[1]) < 1e-5) {  // 左面（y = min_y）
			pointFaces[i] = 3;
			left.push_back(i);
			for (int j = 0; j < numPoints; ++j) {
				if (std::abs(Vertex.row(j)[1] - max_pt[1]) < 1e-5 && Vertex.row(j)[0] == point[0] && Vertex.row(j)[2] == point[2]) {
					symmetricPointIds[i] = j;
					break;
				}
			}
		}
		else if (std::abs(point[0] - min_pt[0]) < 1e-5) {  // 后面（x = min_x）
			pointFaces[i] = 1;
			back.push_back(i);
			for (int j = 0; j < numPoints; ++j) {
				if (std::abs(Vertex.row(j)[0] - max_pt[0]) < 1e-5 && Vertex.row(j)[1] == point[1] && Vertex.row(j)[2] == point[2]) {
					symmetricPointIds[i] = j;
					break;
				}
			}
		} 
		else if (std::abs(point[1] - max_pt[1]) < 1e-5) {  // 右面（y = max_y）
			pointFaces[i] = 4;
			right.push_back(i);
			for (int j = 0; j < numPoints; ++j) {
				if (std::abs(Vertex.row(j)[1] - min_pt[1]) < 1e-5 && Vertex.row(j)[0] == point[0] && Vertex.row(j)[2] == point[2]) {
					symmetricPointIds[i] = j;
					break;
				}
			}
		}
		else if (std::abs(point[2] - max_pt[2]) < 1e-5) {  // 顶面（z = max_z）
			pointFaces[i] = 5;
			for (int j = 0; j < numPoints; ++j) {
				if (std::abs(Vertex.row(j)[2] - min_pt[2]) < 1e-5 && Vertex.row(j)[0] == point[0] && Vertex.row(j)[1] == point[1]) {
					symmetricPointIds[i] = j;
					break;
				}
			}
		}
		else if (std::abs(point[2] - min_pt[2]) < 1e-5) {  // 底面（z = min_z）
			pointFaces[i] = 6;
			for (int j = 0; j < numPoints; ++j) {
				if (std::abs(Vertex.row(j)[2] - max_pt[2]) < 1e-5 && Vertex.row(j)[0] == point[0] && Vertex.row(j)[1] == point[1]) {
					symmetricPointIds[i] = j;
					break;
				}
			}
		}
	}

	return;
}

Eigen::RowVector3d CalIntersection(Eigen::RowVector3d A, Eigen::RowVector3d B, Eigen::RowVector3d C, Eigen::RowVector3d D) {
	Eigen::RowVector3d AC = C - A;
	Eigen::RowVector3d BD = D - B;
	Eigen::MatrixXd M(3, 2);
	M.col(0) = AC.transpose();
	M.col(1) = -BD.transpose();
	Eigen::Vector3d rhs = (B - A).transpose();
	Eigen::Vector2d res = M.jacobiSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(rhs);
	double t = res(0);
	Eigen::RowVector3d intersection = A + t * AC;
	return intersection;
}

struct cmp
{
	bool operator ()(const Eigen::RowVector3i& e1, const Eigen::RowVector3i& e2) const {
		if (e1(0) != e2(0)) return e1(0) < e2(0);
		else if (e1(1) != e2(1)) return e1(1) < e2(1);
		return e1(2) < e2(2);
	}
};

/* 暴力去重 */
//void Imprint::RemoveDuplicate(Mesh& mesh, double eps) {
//	auto& V = mesh.Vertex;
//	auto& T = mesh.Topo;
//
//	std::map<int, int> id_map;
//	std::vector<Eigen::RowVector3d> new_vt;
//	int idx = 0;
//
//	for (int i = 0; i < V.rows(); i++) {
//		bool isDup = false;
//		for (int j = 0; j < new_vt.size(); j++) {
//			if ((V.row(i) - new_vt[j]).squaredNorm() < eps) {
//				id_map[i] = j;
//				isDup = true;
//				break;
//			}
//		}
//		if (!isDup) {
//			new_vt.push_back(V.row(i));
//			id_map[i] = idx++;
//		}
//	}
//
//	V.resize(new_vt.size(), 3);
//	for (int i = 0; i < V.rows(); i++) {
//		V.row(i) = new_vt[i];
//	}
//
//	for (int i = 0; i < T.rows(); i++) {
//		for (int j = 0; j < T.cols(); j++) {
//			T(i, j) = id_map[T(i, j)];
//		}
//	}
//
//	std::set<Eigen::RowVector3i, cmp> new_topo;
//	for (int i = 0; i < T.rows(); i++) {
//		Eigen::RowVector3i temp = T.row(i);
//		std::sort(temp.data(), temp.data() + 3);
//		new_topo.insert(temp);
//	}
//
//	T.resize(new_topo.size(), 3);
//	idx = 0;
//	for (const auto& tri : new_topo) {
//		T.row(idx++) = tri;
//	}
//}
struct PointHash {
	size_t operator()(const Eigen::Vector3d& p) const {
		return std::hash<double>()(p[0]) ^ std::hash<double>()(p[1]) ^ std::hash<double>()(p[2]);
	}
};

static bool isClose(const Eigen::Vector3d& p1, const Eigen::Vector3d& p2, double eps) {
	return (p1 - p2).norm() < eps;
}

void Imprint::RemoveDuplicate(Mesh& mesh, double eps) {
	auto& Vertex = mesh.Vertex;
	auto& Topo = mesh.Topo;
	std::unordered_map<Eigen::Vector3d, int, PointHash> pointMap;
	std::vector<Eigen::Vector3d> uniqueVertices;
	std::vector<int> newIndices(Vertex.rows());

	// Iterate through all vertices
	for (int i = 0; i < Vertex.rows(); ++i) {
		Eigen::Vector3d point(Vertex(i, 0), Vertex(i, 1), Vertex(i, 2));

		bool foundDuplicate = false;
		for (size_t j = 0; j < uniqueVertices.size(); ++j) {
			if (isClose(point, uniqueVertices[j], eps)) {
				newIndices[i] = j; // Point already exists, use its new index
				foundDuplicate = true;
				break;
			}
		}

		if (!foundDuplicate) {
			newIndices[i] = uniqueVertices.size();
			uniqueVertices.push_back(point); // Add new unique point
		}
	}

	// Update Vertex with unique vertices
	Vertex = Eigen::MatrixXd(uniqueVertices.size(), 3);
	for (size_t i = 0; i < uniqueVertices.size(); ++i) {
		Vertex(i, 0) = uniqueVertices[i][0];
		Vertex(i, 1) = uniqueVertices[i][1];
		Vertex(i, 2) = uniqueVertices[i][2];
	}

	// Update Topo by replacing old indices with new ones
	for (int i = 0; i < Topo.rows(); ++i) {
		for (int j = 0; j < Topo.cols(); ++j) {
			Topo(i, j) = newIndices[Topo(i, j)];
		}
	}
}
//void Imprint::RemoveDuplicate(Mesh& mesh, double eps) {
//	auto& vt = mesh.Vertex;
//	auto& topo = mesh.Topo;
//	auto& att = mesh.attribute;
//	auto& mask = mesh.masks;
//
//	const int dim = vt.cols();
//	double reference_length = std::numeric_limits<double>::max();
//
//	// 计算 reference_length
//	for (int i = 0; i < topo.rows(); ++i) {
//		reference_length = std::min(reference_length, (vt.row(topo(i, 0)) - vt.row(topo(i, 1))).norm());
//		reference_length = std::min(reference_length, (vt.row(topo(i, 1)) - vt.row(topo(i, 2))).norm());
//		reference_length = std::min(reference_length, (vt.row(topo(i, 0)) - vt.row(topo(i, 2))).norm());
//	}
//
//	// 计算包围盒
//	std::array<double, 6> box;  // 假设 dim = 3
//	for (int k = 0; k < dim; ++k) {
//		box[k] = std::numeric_limits<double>::max();
//		box[k + dim] = -std::numeric_limits<double>::max();
//	}
//
//	for (int i = 0; i < vt.rows(); ++i) {
//		for (int k = 0; k < dim; ++k) {
//			box[k] = std::min(box[k], vt(i, k));
//			box[k + dim] = std::max(box[k + dim], vt(i, k));
//		}
//	}
//
//	// 旧索引到新索引的映射
//	std::vector<size_t> old_new_map(vt.rows());
//	size_t before = -1;
//	TiGER::common::BinaryTree<Eigen::RowVector3d> binary_tree(box);
//	size_t id = 0;
//	size_t cnt = 0;
//
//	for (int i = 0; i < vt.rows(); ++i) {
//		Eigen::RowVector3d current_vertex = vt.row(i);
//		std::vector<size_t> result;
//		binary_tree.query(current_vertex, reference_length / 2, result, before);
//
//		if (!result.empty()) {
//			if (result.size() > 1) {
//				throw std::runtime_error("too many duplicate points");
//			}
//			old_new_map[id] = result[0];
//		}
//		else {
//			binary_tree.insert(current_vertex, cnt);
//			old_new_map[id] = cnt;
//			cnt++;
//		}
//		id++;
//	}
//
//	// 更新拓扑
//	for (int i = 0; i < topo.rows(); ++i) {
//		for (int k = 0; k < dim; ++k) {
//			topo(i, k) = old_new_map[topo(i, k)];
//		}
//	}
//
//	//std::set<Eigen::RowVector3i, cmp> new_topo;
//	//for (int i = 0; i < F.rows(); i++) {
//	//	Eigen::RowVector3i temp = F.row(i);
//	//	std::sort(temp.data(), temp.data() + 3);
//	//	new_topo.insert(temp);
//	//}
//
//	//F.resize(new_topo.size(), 3);
//	//int idx = 0;
//	//for (const auto& tri : new_topo) {
//	//	F.row(idx++) = tri;
//	//}
//
//	//std::map<Eigen::RowVector3i, std::vector<int>, cmp> t_a;
//	//std::map<Eigen::RowVector3i, std::vector<int>, cmp> t_m;
//	//for (int i = 0; i < topo.rows(); i++) {
//	//	Eigen::RowVector3i temp = topo.row(i);
//	//	std::sort(temp.data(), temp.data() + 3);
//	//	if (t_a.find(temp) != t_a.end()) {
//	//		t_a[temp].insert(t_a[temp].end(), att[i].begin(), att[i].end());
//	//		t_m[temp].insert(t_m[temp].end(), mask[i].begin(), mask[i].end());
//	//	}
//	//	else {
//	//		t_a[temp] = att[i];
//	//		t_m[temp] = mask[i];
//	//	}
//	//}
//
//	//topo.resize(t_a.size(), 3);
//	//att.clear();
//	//att.resize(t_a.size());
//	//mask.clear();
//	//mask.resize(t_m.size());
//	//int idx = 0;
//	//for (const auto& mp : t_a) {
//	//	topo.row(idx) = Eigen::RowVector3i(mp.first[0], mp.first[1], mp.first[2]);
//	//	att[idx] = mp.second;
//	//	++idx;
//	//}
//	//idx = 0;
//	//for (const auto& mp : t_m) {
//	//	mask[idx++] = mp.second;
//	//}
//
//	//// 更新V
//	//for (int i = 0; i < vt.rows(); ++i) {
//	//	vt.row(old_new_map[i]) = vt.row(i);
//	//}
//	//vt.conservativeResize(cnt, Eigen::NoChange);
//}

void Imprint::BottomProject(Mesh& mesh, Parameterization& param) {
	int vnum = tetra_id.size();
	for (int i = 0; i * vnum < mesh.Vertex.rows(); i++) {
		for (auto j : bottom_v_id) {
			Eigen::RowVector3d vt = mesh.Vertex.row(i * vnum + j);
			Eigen::RowVector2d vt_uv;
			param.xyz_to_uv(vt, vt_uv);
			param.uv_to_xyz(vt_uv, vt);
			mesh.Vertex.row(i * vnum + j) = vt;
		}
	}
}


/* 只投影顶面上的点 */
void Imprint::TopProject(Mesh& mesh, Mesh& grid, Parameterization& param) {
	int vnum = tetra_id.size();
	for (int i = 0; i < grid.Topo.rows(); i++) {
		for (auto j : top_v_id) {
			Eigen::RowVector3d vt = mesh.Vertex.row(i * vnum + j);
			Eigen::RowVector3d offset{ grid.f_normal(i, 0), grid.f_normal(i, 1), grid.f_normal(i, 2) };
			Eigen::RowVector3d vt_offset = vt - offset;
			Eigen::RowVector2d vt_offset_uv;
			param.xyz_to_uv(vt_offset, vt_offset_uv);
			param.uv_to_xyz(vt_offset_uv, vt_offset);
			vt = vt_offset + offset;
			mesh.Vertex.row(i * vnum + j) = vt;
		}
	}
}

void Imprint::TopProject(Mesh& mesh, Mesh& grid, Mesh& u_mesh, Parameterization& param) {
	auto& V = mesh.Vertex;
	int vnum = u_mesh.Vertex.rows();
	std::unordered_set<int> doneID;
	double min_z = u_mesh.Vertex(bottom_v_id[0], 2);
	for (auto i : bottom_v_id) doneID.insert(i);
	for (int i = 0; i < grid.Topo.rows(); i++) {
		Eigen::RowVector3d offset{ grid.f_normal(i, 0), grid.f_normal(i, 1), grid.f_normal(i, 2) };
		offset = offset.normalized();
		if (grid.quad_connect(i, 0) < i && grid.quad_connect(i, 0) != -1) {
			for (auto vtid : left) {
				V.row(i * vnum + vtid) = V.row(grid.quad_connect(i, 0) * vnum + symmetricPointIds[vtid]);
				doneID.insert(vtid);
			}
		}
		if (grid.quad_connect(i, 1) < i && grid.quad_connect(i, 1) != -1) {
			for (auto vtid : front) {
				V.row(i * vnum + vtid) = V.row(vnum * grid.quad_connect(i, 1) + symmetricPointIds[vtid]);
				doneID.insert(vtid);
			}
		}
		if (grid.quad_connect(i, 2) < i && grid.quad_connect(i, 2) != -1) {
			for (auto vtid : right) {
				V.row(i * vnum + vtid) = V.row(vnum * grid.quad_connect(i, 2) + symmetricPointIds[vtid]);
				doneID.insert(vtid);
			}
		}
		if (grid.quad_connect(i, 3) < i && grid.quad_connect(i, 3) != -1) {
			for (auto vtid : back) {
				V.row(i * vnum + vtid) = V.row(vnum * grid.quad_connect(i, 3) + symmetricPointIds[vtid]);
				doneID.insert(vtid);
			}
		}
		for (int j = 0; j < vnum; j++) {
			if (doneID.find(j) != doneID.end()) continue;
			Eigen::RowVector3d vt = mesh.Vertex.row(i * vnum + j);
			double height = (u_mesh.Vertex(j, 2) - min_z);
			Eigen::RowVector3d vt_offset = vt - height * offset;
			Eigen::RowVector2d vt_offset_uv;
			param.xyz_to_uv(vt_offset, vt_offset_uv);
			param.uv_to_xyz(vt_offset_uv, vt_offset);
			vt = vt_offset + height * offset;
			mesh.Vertex.row(i * vnum + j) = vt;
		}
	}
}

void Imprint::UnitImprint(Mesh& grid, Mesh& u_mesh, Mesh& output) {
	UnitMeshPre(u_mesh);
	auto& V = grid.Vertex;
	auto& T = grid.Topo;
	auto& uV = u_mesh.Vertex;
	auto& uT = u_mesh.Topo;
	Eigen::MatrixXd V_Tri(T.rows() * uV.rows(), 3);
	Eigen::MatrixXi T_Tri(T.rows() * uT.rows(), 3);
	//Eigen::MatrixXd V_Tri;
	//Eigen::MatrixXi T_Tri;
	for (int i = 0; i < T.rows(); i++) {
		Eigen::MatrixXd tri_pt(uV.rows(), 3);
		std::vector<Eigen::RowVector3d> tetra_pt;

		for (int j = 0; j < 8; j++) {
			tetra_pt.push_back(V.row(T(i, j)));
		}
		tetra_pt.push_back((CalIntersection(tetra_pt[0], tetra_pt[1], tetra_pt[2], tetra_pt[3]) + CalIntersection(tetra_pt[4], tetra_pt[5], tetra_pt[6], tetra_pt[7])) / 2);

		for (int k = 0; k < uV.rows(); k++) {
			tri_pt.row(k) = tetra_pt[tetra_topo(tetra_id[k], 0)] * barycentric_coords[k](0) + tetra_pt[tetra_topo(tetra_id[k], 1)] * barycentric_coords[k](1)
				+ tetra_pt[tetra_topo(tetra_id[k], 2)] * barycentric_coords[k](2) + tetra_pt[tetra_topo(tetra_id[k], 3)] * barycentric_coords[k](3);
		}

		if (i == 0) {
			V_Tri = tri_pt;
			T_Tri = uT;
		}
		else {
			T_Tri.conservativeResize(T_Tri.rows() + uT.rows(), Eigen::NoChange);
			T_Tri.bottomRows(uT.rows()) = uT.array() + V_Tri.rows();
			V_Tri.conservativeResize(V_Tri.rows() + tri_pt.rows(), Eigen::NoChange);
			V_Tri.bottomRows(tri_pt.rows()) = tri_pt;
		}

		//int start_V = i * uV.rows();
		//int start_T = i * uT.rows();
		//V_Tri.block(start_V, 0, uV.rows(), uV.cols()) = tri_pt;
		//T_Tri.block(start_T, 0, uT.rows(), uT.cols()) = uT.array() + start_V;

		for (auto t : u_mesh.attribute) {
			output.attribute.push_back(t);
			output.masks.push_back(std::vector<int>{i});
		}
	}

	output.Vertex = V_Tri;
	output.Topo = T_Tri;
}