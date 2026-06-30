#pragma once
#ifndef CGNSWRITE_H
#define CGNSWRITE_H


#include "mesh_data_struct.h"
#include <string>

using namespace TiGER;

namespace MESHIO
{
	int writeCGNS(const char *filename, SurfaceMesh &sm, VolumeMesh &vm);
	int writeCGNS(const char* filename, SurfaceMesh& sm);
}

#include "cgnsWrite.cpp"
#endif // !CGNEWRITE_H