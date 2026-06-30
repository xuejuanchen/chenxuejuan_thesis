#ifndef PARAMETERIZATION_H
#define PARAMETERIZATION_H
#include "Eigen/Dense"
#include "interface.h"
#include "binary_tree.hpp"
#include <igl/triangle/scaf.h>
#include <igl/arap.h>
#include <igl/boundary_loop.h>
#include <igl/harmonic.h>
#include <igl/map_vertices_to_circle.h>
#include <igl/readOBJ.h>
#include <igl/Timer.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/MappingEnergyType.h>
#include <igl/doublearea.h>
#include <igl/PI.h>
#include <igl/flipped_triangles.h>
#include <igl/topological_hole_fill.h>
#include <igl/lscm.h>
#include <array>
#include <map>

struct Parameterization
{
	Eigen::MatrixXd V;
	Eigen::MatrixXi F;
	Eigen::MatrixXd V_uv;
	igl::triangle::SCAFData scaf_data;
	ProjectIO io;
	Mesh uvh_prism;
	int iter_num = 5;
	float uv_scale = 0.2f;
	bool show_uv = false;
	bool show_airmesh = false;
	std::array<double, 4> uv_range;// {min u, max u, min v, max v}
	
	/* */
	int arap_parameterization(Mesh& mesh);
	int lscm_parameterization(Mesh& mesh);
	void mesh_viewer();
	int get_uvh_prism();
	int xyz_to_uv(Eigen::RowVector3d& xyz, Eigen::RowVector2d& uv);
	int uv_to_xyz(Eigen::RowVector2d& uv, Eigen::RowVector3d& xyz);
	int xyz_to_uvh();
	Eigen::RowVector3d uvh_to_xyz(Eigen::RowVector3d& uvh);
	int grid_generate(MapParameters& mp, Mesh& mesh);
private:
	bool key_down(igl::opengl::glfw::Viewer& viewer, unsigned char key, int modifier);
	TiGER::common::BinaryTree<Eigen::Matrix<double, 3, 3>> xyz_tree;
	TiGER::common::BinaryTree<Eigen::Matrix<double, 3, 3>> uv_tree;
	void init_xyz_tree();
	void init_uv_tree();
};

#endif