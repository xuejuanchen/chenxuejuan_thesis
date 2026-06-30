#include "meshIO.h"
#include "../libigl/include/igl/boundary_loop.h"
#include "twoPointsToLineMesh.hpp"
#include "geometry_interface_plane.h"
#include "discretize_surface.h"
#include "discretize_curve.h"
#include "tiger_parameters.h"
#include "merge_surface_mesh.hpp"
#include "matrix_to_list.hpp"
#include "tetrahedral_mesh.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <math.h>

using namespace TiGER;

double distTowp(point p1, point p2)
{
	double dis = std::pow((p1.x - p2.x), 2) + std::pow((p1.y - p2.y), 2) + std::pow((p1.z - p2.z), 2);
	return std::sqrt(dis);
}

double distTowp(std::array<double, 3> p1, std::array<double, 3> p2)
{
	double dis = std::pow((p1[0] - p2[0]), 2) + std::pow((p1[1] - p2[1]), 2) + std::pow((p1[2] - p2[2]), 2);
	return std::sqrt(dis);
}

//static std::array<double, 3> crossProduct(const std::array<double, 3>& a, const std::array<double, 3>& b)
//{
//	return{
//		a[1] * b[2] - a[2] * b[1],
//		a[2] * b[0] - a[0] * b[2],
//		a[0] * b[1] - a[1] * b[0] };
//}

int main()
{
	std::string filename = "D:/mesh/hill/example/shan.stl";
	std::string filename_vtk = "D:/mesh/hill/example/shan_vtk.vtk";
	std::string mark_pattern = "surface_id";
	Mesh mesh;
	MESHIO::readSTL(filename, mesh);
	//MESHIO::writeVTK(filename_vtk, mesh, mark_pattern);
	//std::cout << mesh.Vertex.rows()<< " " << mesh.Masks.rows() << " " << mesh.Topo.rows() << std::endl;
	std::vector<int> listvLoop;
	igl::boundary_loop(mesh.Topo, listvLoop);
	listvLoop.push_back(0);

	/* 3**2
	*  0**1
	*/
	int cornv_id[5] = { 0, 240590, 241080, 490 , 0};

	/* find surface boundary corner vertex*/
	int minz = mesh.Vertex(0, 2);
	for (int i = 0; i < listvLoop.size(); i++)
	{
		if (mesh.Vertex(listvLoop[i], 2) < minz)
			minz = mesh.Vertex(listvLoop[i], 2);
	}

	/* eight corner vertex*/
	std::vector<point> vertex(8);
	vertex[0].x = mesh.Vertex(0, 0), vertex[0].y = mesh.Vertex(0, 1), vertex[0].z = minz;
	vertex[1].x = mesh.Vertex(240590, 0), vertex[1].y = mesh.Vertex(240590, 1), vertex[1].z = minz;
	vertex[2].x = mesh.Vertex(241080, 0), vertex[2].y = mesh.Vertex(241080, 1), vertex[2].z = minz;
	vertex[3].x = mesh.Vertex(490, 0), vertex[3].y = mesh.Vertex(490, 1), vertex[3].z = minz;

	double height = (distTowp(vertex[0], vertex[1]) + distTowp(vertex[0], vertex[3]) + distTowp(vertex[2], vertex[3]) + distTowp(vertex[1], vertex[2])) / 4;
	double vertex_z = minz + height;
	std::cout << height << " " << vertex_z << std::endl;
	vertex[4].x = mesh.Vertex(0, 0), vertex[4].y = mesh.Vertex(0, 1), vertex[4].z = vertex_z;
	vertex[5].x = mesh.Vertex(240590, 0), vertex[5].y = mesh.Vertex(240590, 1), vertex[5].z = vertex_z;
	vertex[6].x = mesh.Vertex(241080, 0), vertex[6].y = mesh.Vertex(241080, 1), vertex[6].z = vertex_z;
	vertex[7].x = mesh.Vertex(490, 0), vertex[7].y = mesh.Vertex(490, 1), vertex[7].z = vertex_z;

	//for (int i = 0; i < 8; i++)
	//{
	//	std::cout << vertex[i].x << " " << vertex[i].y << " " << vertex[i].z << std::endl;
	//}

	/* vertex id of surface*/
	std::vector<std::vector<int>> sf_vertex_id(5);
	for (int i = 0; i < 3; i++)
		sf_vertex_id[i] = std::vector<int>{ i, i + 1, i + 5, i + 4 };
	sf_vertex_id[3] = std::vector<int>{3, 0, 4, 7};
	sf_vertex_id[4] = std::vector<int>{4, 5, 6, 7};

	/* creat plane */
	TiGER::Geometry geo;
	std::vector<std::vector<LineMesh>> sf_boundary;
	std::vector<std::array<double, 3>> sf_normal(5);
	for (int i = 0; i < sf_vertex_id.size(); i++)
	{
		double x0 = vertex[sf_vertex_id[i][0]].x, y0 = vertex[sf_vertex_id[i][0]].y, z0 = vertex[sf_vertex_id[i][0]].z;
		double x1 = vertex[sf_vertex_id[i][1]].x, y1 = vertex[sf_vertex_id[i][1]].y, z1 = vertex[sf_vertex_id[i][1]].z;
		double x2 = vertex[sf_vertex_id[i][3]].x, y2 = vertex[sf_vertex_id[i][3]].y, z2 = vertex[sf_vertex_id[i][3]].z;
		std::array<double, 3> p0 = { x0, y0, z0 };
		
		double length1 = sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0) + (z1 - z0) * (z1 - z0));
		double length2 = sqrt((x2 - x0) * (x2 - x0) + (y2 - y0) * (y2 - y0) + (z2 - z0) * (z2 - z0));

		std::array<double, 3>basis1 = { (x1 - x0), (y1 - y0), (z1 - z0) };
		std::array<double, 3>basis2 = { (x2 - x0), (y2 - y0), (z2 - z0) };
		sf_normal[i] = crossProduct(basis1, basis2);
		TiGER::GeometrySurface sf;

		creatPlane(p0, basis1, basis2, sf);
		std::shared_ptr<GeometrySurface> surface = std::make_shared<GeometrySurface>(sf);
		geo.surfaces_.push_back(surface);
	}

	/* line mesh*/
	std::vector<point> vertex_linemesh(8);
	vertex_linemesh[0].x = mesh.Vertex(0, 0), vertex_linemesh[0].y = mesh.Vertex(0, 1), vertex_linemesh[0].z = mesh.Vertex(0, 2);
	vertex_linemesh[1].x = mesh.Vertex(240590, 0), vertex_linemesh[1].y = mesh.Vertex(240590, 1), vertex_linemesh[1].z = mesh.Vertex(240590, 2);
	vertex_linemesh[2].x = mesh.Vertex(241080, 0), vertex_linemesh[2].y = mesh.Vertex(241080, 1), vertex_linemesh[2].z = mesh.Vertex(241080, 2);
	vertex_linemesh[3].x = mesh.Vertex(490, 0), vertex_linemesh[3].y = mesh.Vertex(490, 1), vertex_linemesh[3].z = mesh.Vertex(490, 2);
	vertex_linemesh[4].x = mesh.Vertex(0, 0), vertex_linemesh[4].y = mesh.Vertex(0, 1), vertex_linemesh[4].z = vertex_z;
	vertex_linemesh[5].x = mesh.Vertex(240590, 0), vertex_linemesh[5].y = mesh.Vertex(240590, 1), vertex_linemesh[5].z = vertex_z;
	vertex_linemesh[6].x = mesh.Vertex(241080, 0), vertex_linemesh[6].y = mesh.Vertex(241080, 1), vertex_linemesh[6].z = vertex_z;
	vertex_linemesh[7].x = mesh.Vertex(490, 0), vertex_linemesh[7].y = mesh.Vertex(490, 1), vertex_linemesh[7].z = vertex_z;
	// four surface with hill edge
	int intervals = 100;
	int k = 0;
	for (int i = 0; i < 4; i++)
	{
		std::vector<LineMesh> segements;

		// hill line
		for (int j = k; j < listvLoop.size() - 1; j++)
		{
			LineMesh lmsh;
			std::array<double, 3> temp1 = { mesh.Vertex(listvLoop[j], 0), mesh.Vertex(listvLoop[j], 1), mesh.Vertex(listvLoop[j], 2) };
			lmsh.coord.push_back(temp1);
			std::array<double, 3> temp2 = { mesh.Vertex(listvLoop[j + 1], 0), mesh.Vertex(listvLoop[j + 1], 1), mesh.Vertex(listvLoop[j + 1], 2) };
			lmsh.coord.push_back(temp2);
			lmsh.regions.push_back(i);
			std::array<int, 2> seg = { 0, 1 };
			lmsh.segments.push_back(seg);
			double len = distTowp(temp1, temp2);
			std::array<double, 3> p_normal_temp = { (temp2[0] - temp1[0]) / len, (temp2[1] - temp1[1]) / len, (temp2[2] - temp1[2]) / len };
			lmsh.point_normal.push_back(p_normal_temp);
			lmsh.point_normal.push_back(p_normal_temp);
			segements.push_back(lmsh);
			k = j + 1;
			if (listvLoop[j + 1] == cornv_id[i + 1]) break;
		}

		LineMesh lmesh1;
		twoPointsToLineMesh(vertex_linemesh[sf_vertex_id[i][1]], vertex_linemesh[sf_vertex_id[i][2]], lmesh1, intervals, i, sf_normal[i]);
		segements.push_back(lmesh1);
		LineMesh lmesh2;
		twoPointsToLineMesh(vertex_linemesh[sf_vertex_id[i][2]], vertex_linemesh[sf_vertex_id[i][3]], lmesh2, intervals, i, sf_normal[i]);
		segements.push_back(lmesh2);
		LineMesh lmesh3;
		twoPointsToLineMesh(vertex_linemesh[sf_vertex_id[i][3]], vertex_linemesh[sf_vertex_id[i][0]], lmesh3, intervals, i, sf_normal[i]);
		segements.push_back(lmesh3);

		sf_boundary.push_back(segements);
	}
	// top surface
	std::vector<LineMesh> segements(4);
	twoPointsToLineMesh(vertex[sf_vertex_id[4][0]], vertex[sf_vertex_id[4][1]], segements[0], intervals, 4, sf_normal[4]);
	twoPointsToLineMesh(vertex[sf_vertex_id[4][1]], vertex[sf_vertex_id[4][2]], segements[1], intervals, 4, sf_normal[4]);
	twoPointsToLineMesh(vertex[sf_vertex_id[4][2]], vertex[sf_vertex_id[4][3]], segements[2], intervals, 4, sf_normal[4]);
	twoPointsToLineMesh(vertex[sf_vertex_id[4][3]], vertex[sf_vertex_id[4][0]], segements[3], intervals, 4, sf_normal[4]);
	sf_boundary.push_back(segements);

	/* surface mesh */
	std::vector<TiGER::SurfaceMesh> sf_mesh;
	TiGER::IsotropicSurfaceParametersTri args;
	//TiGER::CurveParametersDimension args;
	//args.setdelta_s(0.1);
	//args.setmax_dimension(300);
	SizingFunction size_field = [](const double& x, const double& y,
		const double& z) { return 400; };

	//for (int i = 0; i < 5; i++)
	//{
	//	TiGER::SurfaceMesh sf_mesh_temp;
	//	TiGER::discretize_surface::discretizeSurfaceTri(
	//		geo.surfaces_[i], sf_boundary[i], args, size_field, sf_mesh_temp);
	//	sf_mesh.push_back(sf_mesh_temp);
	//}
	TiGER::SurfaceMesh sf_mesh_temp;
	TiGER::discretize_surface::discretizeSurfaceTri(
			geo.surfaces_[1], sf_boundary[1], args, size_field, sf_mesh_temp);
	sf_mesh.push_back(sf_mesh_temp);
	args.setOutputInformation(true);
	args.setSavepath("D:/mesh/hill/example/xj_bug");
	//printf("%d", sf_boundary[1].size());
	//TiGER::discretize_surface::discretizeSurfaceTri(
	//	geo.surfaces_[1], sf_boundary[1], args, size_field, sf_mesh_temp);
	//sf_mesh.push_back(sf_mesh_temp);
	/* merge surface */
	TiGER::SurfaceMesh sf_hill;
	TiGER::matrix_to_list<3>(mesh.Vertex, sf_hill.coords);
	TiGER::matrix_to_list<3>(mesh.Topo, sf_hill.tris);
	TiGER::matrix_to_list<1>(mesh.Masks, sf_hill.attribute_int);

	sf_mesh.push_back(sf_hill);
	TiGER::SurfaceMesh sf_mesh_output;
	TiGER::merge_surface_mesh(sf_mesh, sf_mesh_output);

	TiGER::Mesh mesh_vtk_output;
	TiGER::list_to_matrix<3>(sf_mesh_output.coords, mesh_vtk_output.Vertex);
	TiGER::list_to_matrix<3>(sf_mesh_output.tris, mesh_vtk_output.Topo);
	TiGER::list_to_matrix<1>(sf_mesh_output.attribute_int, mesh_vtk_output.Masks);
	std::string filename_1 = "D:/mesh/hill/example/shan_box_vtk.vtk";
	MESHIO::writeVTK(filename_1, mesh_vtk_output, "surface_id");

	/* volume mesh */
	TiGER::VolumeMesh vol;
	TiGER::TetrahedraParameters args_vol;
	TiGER::tetrahedral_mesh::constrainDelaunay(sf_mesh_output, args_vol, vol, NULL);

	return 0;
}