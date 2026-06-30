#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

#include <vector>
#include <array>
#include <string>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include "interface.h"
#include "stl_reader.h"

int readParams(const std::string& filename, MapParameters& mp);
int readNAS(const std::string& filename, Mesh& mesh);
int readNAS_newversion(const std::string& filename, Mesh& mesh);
int readSTL(std::string filename, Mesh& mesh);
int readOBJ(std::string filename, Mesh& mesh);
int readVTK(std::string filename, Mesh& mesh, std::string mark_pattern);
int writeVTK(std::string filename, const Mesh& mesh, std::string mark_pattern);
int writeVTK_grid(int Im, int Jm, const std::vector<Eigen::RowVector2d>& vertices, const std::string& filename);
int writeVTK_uv(std::string filename, const Mesh& mesh, std::string mark_pattern);

#include "geometry.cpp"
#endif // _GEOMETRY_HPP_
