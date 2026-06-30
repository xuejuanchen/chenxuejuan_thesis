#include "Parameterization.h"
#include "igl/matrix_to_list.h"
#include "igl/per_face_normals.h"
#include <math.h>
#include <set>
#include <map>
#include <unordered_map>
#include <unordered_set>

int Parameterization::arap_parameterization(Mesh& mesh) {
    V = mesh.Vertex;
    F = mesh.Topo;
    //V_uv = mesh.V_uv;

    Eigen::MatrixXd bnd_uv, uv_init;

    Eigen::VectorXd M;
    igl::doublearea(V, F, M);
    std::vector<std::vector<int>> all_bnds;
    igl::boundary_loop(F, all_bnds);

    // Heuristic primary boundary choice: longest
    auto primary_bnd = std::max_element(all_bnds.begin(), all_bnds.end(), [](const std::vector<int>& a, const std::vector<int>& b) { return a.size() < b.size(); });

    Eigen::VectorXi bnd = Eigen::Map<Eigen::VectorXi>(primary_bnd->data(), primary_bnd->size());

    igl::map_vertices_to_circle(V, bnd, bnd_uv);
    bnd_uv *= sqrt(M.sum() / (2 * igl::PI));
    if (all_bnds.size() == 1)
    {
        if (bnd.rows() == V.rows()) // case: all vertex on boundary
        {
            uv_init.resize(V.rows(), 2);
            for (int i = 0; i < bnd.rows(); i++)
                uv_init.row(bnd(i)) = bnd_uv.row(i);
        }
        else
        {
            igl::harmonic(V, F, bnd, bnd_uv, 1, uv_init);
            if (igl::flipped_triangles(uv_init, F).size() != 0)
                igl::harmonic(F, bnd, bnd_uv, 1, uv_init); // fallback uniform laplacian
        }
    }
    else
    {
        // if there is a hole, fill it and erase additional vertices.
        all_bnds.erase(primary_bnd);
        Eigen::MatrixXi F_filled;
        igl::topological_hole_fill(F, bnd, all_bnds, F_filled);
        igl::harmonic(F_filled, bnd, bnd_uv, 1, uv_init);
        uv_init.conservativeResize(V.rows(), 2);
    }

    Eigen::VectorXi b; Eigen::MatrixXd bc;
    igl::triangle::scaf_precompute(V, F, uv_init, scaf_data, igl::MappingEnergyType::SYMMETRIC_DIRICHLET, b, bc, 0);
    igl::triangle::scaf_solve(scaf_data, iter_num);

    mesh.V_uv = scaf_data.w_uv;
    mesh.T_uv = scaf_data.w_T;

    /* 找到参数域最大最小坐标 */
    //uv_range = { scaf_data.w_uv(F(0,0), 0), scaf_data.w_uv(F(0,0), 0),scaf_data.w_uv(F(0,0), 1), scaf_data.w_uv(F(0,0), 1) };
    uv_range = { scaf_data.w_uv(all_bnds[0][0], 0), scaf_data.w_uv(all_bnds[0][0], 0), scaf_data.w_uv(all_bnds[0][0], 1), scaf_data.w_uv(all_bnds[0][0], 1) };
    for (int i = 0; i < all_bnds.size(); i++) {
        for (int j = 0; j < all_bnds[i].size(); j++) {
            if (scaf_data.w_uv(all_bnds[i][j], 0) < uv_range[0]) uv_range[0] = scaf_data.w_uv(all_bnds[i][j], 0);
            if (scaf_data.w_uv(all_bnds[i][j], 0) > uv_range[1]) uv_range[1] = scaf_data.w_uv(all_bnds[i][j], 0);
            if (scaf_data.w_uv(all_bnds[i][j], 1) < uv_range[2]) uv_range[2] = scaf_data.w_uv(all_bnds[i][j], 1);
            if (scaf_data.w_uv(all_bnds[i][j], 1) > uv_range[3]) uv_range[3] = scaf_data.w_uv(all_bnds[i][j], 1);
        }
    }

    /* 初始化树 */
    Parameterization::init_xyz_tree();
    Parameterization::init_uv_tree();
    return 0;
}

void Parameterization::init_xyz_tree() { xyz_tree.init(scaf_data.m_V, scaf_data.m_T); }

void Parameterization::init_uv_tree() { 
    Eigen::MatrixXd w_uv_extended(scaf_data.w_uv.rows(), scaf_data.w_uv.cols() + 1);
    w_uv_extended.block(0, 0, scaf_data.w_uv.rows(), scaf_data.w_uv.cols()) = scaf_data.w_uv;
    w_uv_extended.col(scaf_data.w_uv.cols()).setZero();
    uv_tree.init(w_uv_extended, scaf_data.w_T);
}

int Parameterization::lscm_parameterization(Mesh& mesh) {
    Eigen::VectorXi bnd, b(2, 1);
    igl::boundary_loop(F, bnd);

    b(0) = bnd(0);
    b(1) = bnd(bnd.size() / 2);

    Eigen::MatrixXd bc(2, 2);
    bc(0, 0) = 0; bc(0, 1) = 0;
    bc(1, 0) = 1; bc(1, 1) = 0;

    V_uv.resize(mesh.Vertex.rows(), 2);
    igl::lscm(V, F, b, bc, V_uv);

    return 1;
}

int Parameterization::xyz_to_uv(Eigen::RowVector3d& xyz, Eigen::RowVector2d& uv) {

    /* 查找所属的三角形id */
    size_t simplex_id;
    double distance;
    Eigen::RowVector3d closet_point;
    Eigen::Vector3d barycentric_coordinates;

    xyz_tree.queryNearestTriangle(xyz, simplex_id, distance, closet_point, barycentric_coordinates);

    /* 转换为uv坐标 */
    std::array<int, 3> nt{ scaf_data.m_T(simplex_id, 0), scaf_data.m_T(simplex_id, 1), scaf_data.m_T(simplex_id, 2) };
    std::array<double, 3> p{ scaf_data.m_V(nt[0], 0), scaf_data.m_V(nt[0], 1), scaf_data.m_V(nt[0], 2) };
    double u = 0;
    double v = 0;
    for (int i = 0; i < 3; i++) {
        u += scaf_data.w_uv(nt[i], 0) * barycentric_coordinates(i);
        v += scaf_data.w_uv(nt[i], 1) * barycentric_coordinates(i);
    }
    uv << u, v;

    //std::cout << "xyz" << xyz(0) << ", " << xyz(1) << ", " << xyz(2) << std::endl;
    //std::cout << "uv" << uv(0) << ", " << uv(1) << std::endl;
    return simplex_id;
}

int Parameterization::uv_to_xyz(Eigen::RowVector2d& uv, Eigen::RowVector3d& xyz) {

    size_t simplex_id;
    double distance;
    Eigen::RowVector3d uv_;
    Eigen::RowVector3d closet_point;
    Eigen::Vector3d barycentric_coordinates;

    for (int i = 0; i < 2; i++)
        uv_(i) = uv(i);
    uv_(2) = 0;
    uv_tree.queryNearestTriangle(uv_, simplex_id, distance, closet_point, barycentric_coordinates);

    if (simplex_id >= scaf_data.m_T.rows()) return -1;

    std::array<int, 3> nt{ scaf_data.w_T(simplex_id, 0), scaf_data.w_T(simplex_id, 1), scaf_data.w_T(simplex_id, 2) };
    double x = 0, y = 0, z = 0;
    for (int i = 0; i < 3; i++) {
        x += scaf_data.m_V(nt[i], 0) * barycentric_coordinates(i);
        y += scaf_data.m_V(nt[i], 1) * barycentric_coordinates(i);
        z += scaf_data.m_V(nt[i], 2) * barycentric_coordinates(i);
    }
    xyz << x, y, z;

    //std::cout << "uv" << uv(0) << ", " << uv(1) << std::endl;
    //std::cout << "xyz" << xyz(0) << ", " << xyz(1) << ", " << xyz(2) << std::endl;
    return simplex_id;
}

/* 计算四面体体积 */
static double TetrahedronVolume(const Eigen::RowVector3d& A, const Eigen::RowVector3d& B, const Eigen::RowVector3d& C, const Eigen::RowVector3d& D) {
    Eigen::Matrix4d mat;
    mat << A[0], A[1], A[2], 1.0,
        B[0], B[1], B[2], 1.0,
        C[0], C[1], C[2], 1.0,
        D[0], D[1], D[2], 1.0;

    return std::abs(mat.determinant()) / 6.0;
}

/* 判断点是否在四面体上 */
static bool IsPointInTetrahedron(const Eigen::RowVector3d& A, const Eigen::RowVector3d& B, const Eigen::RowVector3d& C,
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

//int Parameterization::get_uvh_prism() {
//    auto uV = scaf_data.w_uv;
//    auto uT = scaf_data.w_T;
//    auto& pV = uvh_prism.Vertex;
//    auto& pT = uvh_prism.Topo;
//    pV.resize(2 * uV.rows(), 3);
//    pT.resize(uT.rows(), 6);
//    for (int i = 0; i < uV.rows(); i++) {
//        pV(i, 0) = uV(i, 0), pV(i, 1) = uV(i, 1), pV(i, 2) = 0;
//        pV(uV.rows() + i, 0) = uV(i, 0), pV(uV.rows() + i, 1) = uV(i, 1), pV(uV.rows() + i, 2) = this->io.height;
//    }
//}

/* 三棱柱中四面体的顺序 */
/* 0：0 1 2 4
 * 1：0 2 4 5
 * 2：0 3 4 5
 */

static int whichTet(std::vector<Eigen::RowVector3d>& sixp, const Eigen::RowVector3d& P, Eigen::RowVector4d& barycentricCoords) {
    Eigen::RowVector4d bc;
    if (IsPointInTetrahedron(sixp[0], sixp[1], sixp[2], sixp[4], P, bc)) {
        barycentricCoords = bc;
        return 0;
    }
    else if (IsPointInTetrahedron(sixp[0], sixp[2], sixp[4], sixp[5], P, bc)) {
        barycentricCoords = bc;
        return 1;
    }
    else if (IsPointInTetrahedron(sixp[0], sixp[3], sixp[4], sixp[5], P, bc)) {
        barycentricCoords = bc;
        return 2;
    }
    return -1;
}

Eigen::RowVector3d Parameterization::uvh_to_xyz(Eigen::RowVector3d& uvh) {
    Eigen::RowVector3d xyz;
    auto uV = scaf_data.w_uv;
    auto uT = scaf_data.w_T;
    Eigen::RowVector3d uvh_ = uvh;
    uvh_[2] = 0;
    size_t simplex_id;
    double distance;
    Eigen::RowVector3d closet_point;
    Eigen::Vector3d barycentric_coordinates;
    uv_tree.queryNearestTriangle(uvh_, simplex_id, distance, closet_point, barycentric_coordinates);
    int global_id = this->io.mp[simplex_id];
    std::vector<Eigen::RowVector3d> sixp;
    for (int i = 0; i < 6; i++) {
        if (i < 3) {
            Eigen::RowVector3d temp(uV(uT(simplex_id, i), 0), uV(uT(simplex_id, i), 1), 0);
            sixp.push_back(temp);
        }
        else {
            Eigen::RowVector3d temp(uV(uT(simplex_id, i - 3), 0), uV(uT(simplex_id, i - 3), 1), this->io.height);
            sixp.push_back(temp);
        }
    }
    Eigen::RowVector4d barycentricCoords;
    int tetID = whichTet(sixp, uvh, barycentricCoords);
    std::vector<Eigen::RowVector3d> tetp(4);
    if (tetID == 0) {
        tetp[0] = io.prism.Vertex.row(io.prism.Topo(global_id, 0));
        tetp[1] = io.prism.Vertex.row(io.prism.Topo(global_id, 1));
        tetp[2] = io.prism.Vertex.row(io.prism.Topo(global_id, 2));
        tetp[3] = io.prism.Vertex.row(io.prism.Topo(global_id, 4));
    }
    else if (tetID == 1) {
        tetp[0] = io.prism.Vertex.row(io.prism.Topo(global_id, 0));
        tetp[1] = io.prism.Vertex.row(io.prism.Topo(global_id, 2));
        tetp[2] = io.prism.Vertex.row(io.prism.Topo(global_id, 4));
        tetp[3] = io.prism.Vertex.row(io.prism.Topo(global_id, 5));
    }
    else if (tetID == 2) {
        tetp[0] = io.prism.Vertex.row(io.prism.Topo(global_id, 0));
        tetp[1] = io.prism.Vertex.row(io.prism.Topo(global_id, 3));
        tetp[2] = io.prism.Vertex.row(io.prism.Topo(global_id, 4));
        tetp[3] = io.prism.Vertex.row(io.prism.Topo(global_id, 5));
    }
    xyz = tetp[0] * barycentricCoords(0) + tetp[1] * barycentricCoords(1) + tetp[2] * barycentricCoords(2) + tetp[3] * barycentricCoords(3);
    return xyz;
}

/* 法向光滑化 */
static int sf_normal_smoothed(std::vector<Eigen::RowVector4i>& topo, std::vector<Eigen::RowVector3d>& normal)
{
    ///*  step.1 找到边界上的点
    //    step.2 获取每个点的连接关系
    //    step.3 每个点的法向等于1/2自身法向加上1/2周围法向的平均
    //    step.4 迭代n轮*/

    //获得非边界点
    std::map<int, std::vector<int>> pos_topo;
    for (int i = 0; i < topo.size(); i++) {
        for (int j = 0; j < 4; j++) {
            pos_topo[topo[i](j)].push_back(i);
        }
    }
    std::vector<int> inner_pos;
    for (const auto& entry : pos_topo) {
        if (entry.second.size() == 4) {
            inner_pos.push_back(entry.first);
        }
    }

    // 获得所有点的连接关系
    std::unordered_map<int, std::set<int>> pos_pos;
    for (const auto& cell : topo) {
        for (int i = 0; i < 4; ++i) {
            for (int j = i + 1; j < 4; ++j) {
                // 连接 cell[i] 和 cell[j]
                pos_pos[cell[i]].insert(cell[j]);
                pos_pos[cell[j]].insert(cell[i]);
            }
        }
    }

    // 迭代
    int iteration = 5;
    while (iteration) {
        std::vector<Eigen::RowVector3d> temp;
        temp.assign(normal.begin(), normal.end());
        for (const auto& i : inner_pos) {
            int cnt = 0;
            Eigen::RowVector3d value(0, 0, 0);
            for (const auto& j : pos_pos[i]) {
                value += normal[j];
                cnt++;
            }
            temp[i] = 0.5 * normal[i] + 0.5 * (value / cnt);
            //temp[i] =value / cnt;
        }
        normal.assign(temp.begin(), temp.end());
        iteration--;
    }
    return 1;
}

struct Edge {
    int p1, p2;

    // 构造函数，确保边是无向的
    Edge(int a, int b) {
        if (a < b) {
            p1 = a;
            p2 = b;
        }
        else {
            p1 = b;
            p2 = a;
        }
    }

    // 重载运算符，使得可以在unordered_map中使用Edge作为key
    bool operator==(const Edge& other) const {
        return p1 == other.p1 && p2 == other.p2;
    }
};

// 重载哈希函数，使得可以在unordered_map中使用Edge作为key
namespace std {
    template <>
    struct hash<Edge> {
        size_t operator()(const Edge& edge) const {
            return hash<int>()(edge.p1) ^ hash<int>()(edge.p2);
        }
    };
}
static Eigen::MatrixXi getQuadAdjacency(const std::vector<Eigen::RowVector4i>& Topo) {

    int numQuads = Topo.size();
    // 用于存储每条边与它所对应的四边形
    std::unordered_map<Edge, std::vector<int>> edgeToQuad;

    // 遍历每个四边形，记录每条边
    for (int i = 0; i < numQuads; ++i) {
        // 获取四个顶点
        const Eigen::RowVector4i& quad = Topo[i];
        int a = quad[0];
        int b = quad[1];
        int c = quad[2];
        int d = quad[3];

        // 对于每个四边形的四条边
        edgeToQuad[Edge(d, a)].push_back(i); // 边 da
        edgeToQuad[Edge(a, b)].push_back(i); // 边 ab
        edgeToQuad[Edge(b, c)].push_back(i); // 边 bc
        edgeToQuad[Edge(c, d)].push_back(i); // 边 cd
    }

    // 存储每个四边形的相邻四边形ID，按照左、前、右、后的顺序
    Eigen::MatrixXi adjacencies(numQuads, 4);
    adjacencies.setConstant(-1); // 初始化为 -1，表示没有相邻的单元

    // 遍历每个四边形，找到四条边的相邻四边形
    for (int i = 0; i < numQuads; ++i) {
        // 获取四个顶点
        const Eigen::RowVector4i& quad = Topo[i];
        int a = quad[0];
        int b = quad[1];
        int c = quad[2];
        int d = quad[3];

        // 依次遍历四条边
        std::vector<Edge> edges = { Edge(d, a), Edge(a, b), Edge(b, c), Edge(c, d) };
        for (int j = 0; j < 4; ++j) {
            const Edge& edge = edges[j];

            // 获取当前边对应的四边形
            const std::vector<int>& quads = edgeToQuad[edge];

            // 如果该边有两个四边形，排除当前四边形，找到另一个四边形
            for (int quadId : quads) {
                if (quadId != i) {
                    adjacencies(i, j) = quadId;
                    break;
                }
            }
        }
    }

    return adjacencies;
}

int Parameterization::grid_generate(MapParameters& mp, Mesh& mesh) {
    /* 输入参数xyz转换uv */
    if (!mp.isUV) {
        xyz_to_uv(mp.start_pos, mp.start_pos_uv);
        xyz_to_uv(mp.d1_oritation, mp.d1_o_uv);
        mp.d2_o_uv = Eigen::RowVector2d(-1 * mp.d1_o_uv(1), mp.d1_o_uv(0));
        //param.xyz_to_uv(mp.d2_oritation, mp.d2_o_uv);
    }
    
    /* 归一化 */
    mp.d1_o_uv /= mp.d1_o_uv.norm();
    mp.d2_o_uv /= mp.d2_o_uv.norm();
    
    /* 求解局部坐标和uv坐标转换矩阵 */
    Eigen::Matrix2d T, T_inverse;// unit_point * T + start_pos = uv_point, (uv_point - start_pos) * T_inverse = unit_point;
    Eigen::Matrix2d unit_point;
    unit_point << 0, 1, 1, 0;
    Eigen::Matrix2d uv_point;
    uv_point.row(0) = mp.d1_distance * mp.d1_o_uv, uv_point.row(1) = mp.d2_distance * mp.d2_o_uv;
    T = unit_point.inverse() * uv_point, T_inverse = T.inverse();
    
    /* 转换包围盒坐标 */
    Eigen::RowVector2d box_uv_1;
    box_uv_1 << uv_range[1] - uv_range[0], 0;
    Eigen::RowVector2d box_uv_2;
    box_uv_2 << 0, uv_range[3] - uv_range[2];
    int range = ceil(std::max((box_uv_1 * T_inverse).norm(), (box_uv_2 * T_inverse).norm()));
    Eigen::RowVector2d box_unit((box_uv_1 * T_inverse).norm(), (box_uv_2*T_inverse).norm());
    
    std::vector<Eigen::RowVector2d> grid_pos_origin;// 栅格点
    std::vector<Eigen::RowVector4i> grid_topo_origin;// 栅格拓扑
        
    int i_index = 0;
    for (int i = -1 * range; i < range; i++) {
        int j_index = 0;
        for (int j = -1 * range; j < range; j++) {
            grid_pos_origin.push_back(Eigen::RowVector2d(i, j));
            if (i_index && j_index) {
                grid_topo_origin.push_back(Eigen::RowVector4i((i_index - 1) * 2 * range + j_index - 1, (i_index - 1) * 2 * range + j_index,
                    i_index * 2 * range + j_index, i_index * 2 * range + j_index - 1));
            }
            j_index++;
        }
        i_index++;
    }
    
    for (int i = 0; i < grid_pos_origin.size(); i++) {
        grid_pos_origin[i] = grid_pos_origin[i] * T + mp.start_pos_uv; 
    }
    
    /*  提取出范围之内的点和拓扑以及最初的法向
    调用法向光滑化，生成六面体框架
    方案一：边界点使用边界的两点法向之和，其余的使用该单元的平均法向
    方案二：直接通过光滑化后的法向offset */
    
    /* 删除范围外的点并更新拓扑 */
    /* 删点 */
    Eigen::MatrixXd sf_stl_normal;
    igl::per_face_normals(scaf_data.m_V, scaf_data.m_T, sf_stl_normal);
    std::vector<Eigen::RowVector3d> grid_pos_normal;
    std::map<int, int> idmap; // 删除点前后的id对照
    std::vector<Eigen::RowVector3d> grid_pos;
    for (int i = 0; i < grid_pos_origin.size(); i++) {
        Eigen::RowVector3d temp_xyz;
        int triangle_id = uv_to_xyz(grid_pos_origin[i], temp_xyz);
        if (triangle_id >= 0) {
            grid_pos.push_back(temp_xyz);
            grid_pos_normal.push_back(sf_stl_normal.row(triangle_id));
            idmap[i] = grid_pos.size() - 1;
        }
    }
    /* 更新拓扑 */
    std::vector<Eigen::RowVector4i> grid_topo;
    for (int i = 0; i < grid_topo_origin.size(); i++) {
        bool is = true;
        Eigen::RowVector4i temp;
        for (int j = 0; j < 4; j++) {
            if (idmap.find(grid_topo_origin[i](j)) != idmap.end()) {
                temp(j) = idmap[grid_topo_origin[i](j)];
            }
            else {
                is = false;
                break;
            }
        }
        if (is) {
            grid_topo.push_back(temp);
        }
    }
    /* 法向光滑化 */
    sf_normal_smoothed(grid_topo, grid_pos_normal);
    
    /* 获取栅格拓扑连接关系 */
    mesh.quad_connect = getQuadAdjacency(grid_topo); // rows = grid_topo.rows, cols的顺序左、前、右、后

    std::vector<Eigen::RowVector3d> grid_pos_xyz;
    for (int i = 0; i < grid_pos.size(); i++) {
        Eigen::RowVector3d top_xyz;
        top_xyz = grid_pos[i] + mp.height * grid_pos_normal[i];
        grid_pos_xyz.push_back(grid_pos[i]);
        grid_pos_xyz.push_back(top_xyz);
    }
    
    /* 拓扑 */
    std::vector<Eigen::RowVectorXi> grid_topo_xyz;
    std::vector<Eigen::RowVector3d> offset_normal;
    for (int i = 0; i < grid_topo.size(); i++) {
        Eigen::RowVectorXi temp(8);
        Eigen::RowVector3d f_unit{ 0, 0, 0 };
        for (int j = 0; j < 4; j++) {
            temp(j) = 2 * grid_topo[i](j);
            temp(j + 4) = temp(j) + 1;
            f_unit += grid_pos_normal[grid_topo[i](j)];
        }
        f_unit /= 4;
        offset_normal.push_back(f_unit);
        grid_topo_xyz.push_back(temp);
    }
    
    /* 输出格式转换 */
    mesh.Vertex.resize(grid_pos_xyz.size(), 3);
    mesh.Topo.resize(grid_topo_xyz.size(), 8);
    mesh.f_normal.resize(offset_normal.size(), 3);
    
    for (int i = 0; i < grid_topo_xyz.size(); i ++) {
        mesh.Topo.row(i) = grid_topo_xyz[i];
    }
    
    for (int i = 0; i < grid_pos_xyz.size(); i++) {
        mesh.Vertex.row(i) = grid_pos_xyz[i];
    }
    
    for (int i = 0; i < offset_normal.size(); i++) {
        mesh.f_normal.row(i) = offset_normal[i];
    }
    
    return 1;
}

bool Parameterization::key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier)
{
    if (key == '1')
        show_uv = false, show_airmesh = false;
    else if (key == '2')
        show_uv = true, show_airmesh = false;
    else if (key == '3')
        show_uv = true, show_airmesh = true;
    const auto& V_uv = uv_scale * scaf_data.w_uv.topRows(V.rows());
    if (show_uv)
    {
        if (show_airmesh) {
            viewer.data().clear();
            viewer.data().set_mesh(scaf_data.w_uv, scaf_data.w_T);
            viewer.data().set_uv(scaf_data.w_uv);
            viewer.core().align_camera_center(scaf_data.w_uv, scaf_data.w_T);
        }
        else
        {
            viewer.data().clear();
            viewer.data().set_mesh(V_uv, F);
            viewer.data().set_uv(V_uv);
            viewer.core().align_camera_center(V_uv, F);
        }
    }
    else
    {
        viewer.data().clear();
        viewer.data().set_mesh(V, F);
        viewer.data().set_uv(V_uv);
        viewer.core().align_camera_center(V, F);
    }

    viewer.data().compute_normals();

    return false;
}

void Parameterization::mesh_viewer() {
    // Plot the mesh
    igl::opengl::glfw::Viewer viewer;
    viewer.data().set_mesh(V, F);
    const auto& V_uv = uv_scale * scaf_data.w_uv.topRows(V.rows());
    viewer.data().set_uv(V_uv);
    viewer.callback_key_down = std::bind(&Parameterization::key_down, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    // Enable wireframe
    viewer.data().show_lines = true;

    // Draw checkerboard texture
    viewer.data().show_texture = true;

    //std::cerr << "Press 1 for Mesh, 2 for UV, 3 for UV with airmesh" << std::endl;

    // Launch the viewer
    viewer.launch();
}