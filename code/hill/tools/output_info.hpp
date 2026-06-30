/**
 * @author :yhf
*/
#include "../include/mesh_data_struct.h"
#include "../include/discretize_surface_parameters.h"
#include "./list_to_matrix.hpp"
#include "../include/geometry_data_struct.h"
#include "../include/sizing_field_interface.h"
#include <fstream>
#include <iostream>
#include <string>
namespace TiGER {
void output_discrete_surface_info(
    std::string filename, std::shared_ptr<GeometrySurface>& surface,
    const std::vector<LineMesh>& boundary_segments,
    const IsotropicSurfaceParametersTri& iso_args,
    const SizingFunction& sizefield) {
  FILE* vtk_file = fopen((filename + "_boundary.vtk").c_str(), "w");
  int allPoints = 0;
  for (int k = 0; k < boundary_segments.size(); k++) {
    // allPoints+= cvmshRes[k].cvMshPtNums;
    allPoints += boundary_segments[k].coord.size();
  }
  if (vtk_file == nullptr) {
    std::cout << "Cannot open writecurvevtkfile";
  } else {
    std::cout << "write boundary to" + filename + "_boundary.vtk\n";
    fprintf(vtk_file, "# vtk DataFile Version 2.0\n");
    fprintf(vtk_file, "curve grid\n");
    fprintf(vtk_file, "ASCII\n");
    fprintf(vtk_file, "DATASET UNSTRUCTURED_GRID\n");
    fprintf(vtk_file, "POINTS %d double\n", allPoints);
    for (int i = 0; i < boundary_segments.size(); i++) {
      for (int j = 0; j < boundary_segments[i].coord.size(); j++) {
        double x = boundary_segments[i].coord[j][0];
        double y = boundary_segments[i].coord[j][1];
        double z = boundary_segments[i].coord[j][2];
        fprintf(vtk_file, "%10.5f %10.5f %10.5f\n", x, y, z);
      }
    }

    fprintf(vtk_file, "CELLS %d %d \n", (int)boundary_segments.size(),
            allPoints + (int)boundary_segments.size());
    int cntt = 0;
    for (int i = 0; i < boundary_segments.size(); i++) {
      fprintf(vtk_file, "%d\t", (int)boundary_segments[i].coord.size());
      for (int j = 0; j < boundary_segments[i].coord.size(); j++) {
        fprintf(vtk_file, " %d", cntt++);
      }
      fprintf(vtk_file, "\n");
    }

    fprintf(vtk_file, "CELL_TYPES %d \n", (int)boundary_segments.size());
    for (int i = 0; i < boundary_segments.size(); i++) {
      fprintf(vtk_file, "%d\n", 4);
    }

    fprintf(vtk_file, "CELL_DATA %d\n", (int)boundary_segments.size());
    fprintf(vtk_file, "SCALARS surface_curve_id int 1\n");
    fprintf(vtk_file, "LOOKUP_TABLE curv\n");
    for (int i = 0; i < boundary_segments.size(); i++) {
      fprintf(vtk_file, " %d\n", i + 1);
    }
    fclose(vtk_file);
  }

  FILE* f = fopen(filename.c_str(), "w");
  if (!f) {
    std::cout << "No such file; Output input information of discrete_surface failed\n";
    return;
  }
  std::cout << "Writing discrete surface input to -" + filename << std::endl;
  std::array<double, 4> uv_scale = surface->getUVScaleFunction();
  std::array<double, 2> mid_uv = {(uv_scale[1] + uv_scale[0]) / 2,
                                  (uv_scale[3] + uv_scale[2]) / 2};
  std::array<double, 2> left_mid_uv = {(mid_uv[0] + uv_scale[0]) / 2,
                                       (mid_uv[1] + uv_scale[2]) / 2};
  std::array<double, 2> right_mid_uv = {(mid_uv[0] + uv_scale[1]) / 2,
                                       (mid_uv[1] + uv_scale[3]) / 2};
  std::array<double, 3> mid_pt, mid_du, mid_dv, mid_dudu, mid_dvdv, mid_dudv,
      left_mid_pt, right_mid_pt;
  mid_pt = surface->d0Function(mid_uv);
  left_mid_pt = surface->d0Function(left_mid_uv);
  right_mid_pt = surface->d0Function(right_mid_uv);
  surface->d1Function(mid_uv, mid_du, mid_dv);
  surface->d2Function(mid_uv, mid_dudu, mid_dvdv, mid_dudv);
  fprintf(f, "%10.5f %10.5f %10.5f\n", sizefield(mid_pt[0], mid_pt[1], mid_pt[2]),
          sizefield(left_mid_pt[0], left_mid_pt[1], left_mid_pt[2]),
          sizefield(right_mid_pt[0], right_mid_pt[1], right_mid_pt[2]));
  fprintf(f, "%10.5f %10.5f %10.5f\n", mid_pt[0], mid_pt[1], mid_pt[2]);
  fprintf(f, "%10.5f %10.5f %10.5f\n", mid_du[0], mid_du[1], mid_du[2]);
  fprintf(f, "%10.5f %10.5f %10.5f\n", mid_dv[0], mid_dv[1], mid_dv[2]);
  fprintf(f, "%10.5f %10.5f %10.5f\n", mid_dudu[0], mid_dudu[1], mid_dudu[2]);
  fprintf(f, "%10.5f %10.5f %10.5f\n", mid_dvdv[0], mid_dvdv[1], mid_dvdv[2]);
  fprintf(f, "%10.5f %10.5f %10.5f\n", mid_dudv[0], mid_dudv[1], mid_dudv[2]);
  fprintf(f, "boundary num %d\n", boundary_segments.size());
  for (auto curve : boundary_segments) {
    fprintf(f, "POINTS %d double\n", curve.coord.size());
    for (auto pt : curve.coord) {
      fprintf(f, "%10.5f %10.5f %10.5f\n", pt[0], pt[1], pt[2]);
    }
  }
  fprintf(f, "IsotropicSurfaceParameters\n");
  fprintf(f, "%10.5f %10.5f %10.5f %10.5f %d\n", iso_args.getmMaxAngle(),
          iso_args.getmMaxDeviation(), iso_args.getmUserSpecifiedMaxEdgeLen(),
          iso_args.getmUserSpecifiedMinEdgeLen(),iso_args.getmSwapCellWithNoInteriorPoints());
  fclose(f);
}


void read_discrete_surface_info(std::string filename,
    std::array<double, 3> &sizefiled_info,
    std::vector<std::array<double, 3>> &surface_info,
    std::vector<LineMesh>& boundary_segments,
    IsotropicSurfaceParametersTri& iso_args) {
  std::ifstream f;
  f.open(filename);
  if (!f.is_open()) {
    std::cout << "No such file. - " << filename << std::endl;
    return;
  }
  char buffer[256];
  int bd_cnt;
  while (!f.eof()) {
    for (int j = 0; j < 3; j++) {
      f >> buffer;
      sizefiled_info[j] = std::stod(buffer);
    }
    for (int i = 0; i < 6; i++) {
      std::array<double, 3> sf_info;
      for (int j = 0; j < 3; j++) {
        f >> buffer;
        sf_info[j] = std::stod(buffer);
      }
      surface_info.push_back(sf_info);
    }
    f >> buffer;
    f >> buffer;
    f >> buffer;
    bd_cnt = std::stoi(buffer);
    while (bd_cnt--)
    {
      int pt_cnt;
      f >> buffer;
      f >> buffer;
      pt_cnt = std::stoi(buffer);
      f >> buffer;
      LineMesh line_mesh;
      while (pt_cnt--)
      {
        std::array<double, 3> pt;
        f >> buffer;
        pt[0] = std::stod(buffer);
        f >> buffer;
        pt[1] = std::stod(buffer);
        f >> buffer;
        pt[2] = std::stod(buffer);
        line_mesh.coord.push_back(pt);
      }
      boundary_segments.push_back(line_mesh);
    }
    f >> buffer;
    f >> buffer;
    iso_args.setmMaxAngle(std::stod(buffer));
    f >> buffer;
    iso_args.setmMaxDeviation(std::stod(buffer));
    f >> buffer;
    iso_args.setmUserSpecifiedMaxEdgeLen(std::stod(buffer));
    f >> buffer;
    iso_args.setmUserSpecifiedMinEdgeLen(std::stod(buffer));
     f >> buffer;
    iso_args.setmSwapCellWithNoInteriorPoints(std::stoi(buffer));
     return;
  }
}

}  // namespace TiGER