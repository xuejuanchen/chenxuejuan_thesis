#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#if defined(_WIN32) || defined(_WIN64)
#ifdef IMPRINTOOL_EXPORTS
#define IMPRINTOOL_API __declspec(dllexport)
#else
#define IMPRINTOOL_API __declspec(dllimport)
#endif
#else
#define IMPRINTOOL_API
#endif

#include <vector>
#include <array>
#include <string>
#include <map>
#include <Eigen/Dense>

struct Mesh
{
    Eigen::MatrixXd Vertex; // vertex
    Eigen::MatrixXi Topo; // triangle topo
    Eigen::MatrixXi Masks;

    std::vector<std::vector<int> > attribute; // material id, row = topo.size
    std::vector<std::vector<int> > masks; // unit id, row = topo.size

    // intermediate data
    Eigen::MatrixXd V_uv;
    Eigen::MatrixXi T_uv;
    Eigen::MatrixXd f_normal;
    Eigen::MatrixXd v_normal;
    Eigen::MatrixXi quad_connect; // 四边形栅格连接关系，rows = grid_topo.rows, cols的顺序左、前、右、后
};

struct MapParameters{
    enum mapType{
        single =0,
        multiple 
    } mtype;
    std::vector<int> buttom_tris;

    bool isUV = true;
    Eigen::RowVector3d start_pos;
    Eigen::RowVector2d start_pos_uv;

    Eigen::RowVector3d d1_oritation;
    Eigen::RowVector2d d1_o_uv;
    double d1_distance;

    Eigen::RowVector3d d2_oritation;
    Eigen::RowVector2d d2_o_uv;
    double d2_distance;
    double height;
};

struct ProjectIO{
    // input parameters
    std::string version = "";
    Mesh sf_input;
    Mesh sf_select;
    Mesh prism;
    Mesh unit_input;
    MapParameters paras;
    std::map<int, int> mp;
    double height;
    // output parameters
    Mesh output;
    
};

IMPRINTOOL_API int run(ProjectIO& io);
#endif // !INTERFACE_HPP
