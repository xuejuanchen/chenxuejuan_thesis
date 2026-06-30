#ifndef EXTRUDE_H
#define EXTRUDE_H

#include "./mesh_data_struct.h"
#include "./definitions.h"
#include "./extrude_parameters.h"

#include "meshIO.h"
#include<map>
#include<vector>
#include <igl/remove_unreferenced.h>
#include <geom_func.h>


using namespace std;


namespace TiGER{
    namespace extrude{

        /**
         * @brief 使用前沿层进法生成多边形网格 
         * 
         * @param line_mesh 线网格
         * @param args 参数
         * @param surface_mesh 四边形网格
         */
        Tiger_API void extrude2d(const TiGER::LineMesh& line_mesh,const ExtrudeParameters& args, SurfaceMesh& surface_mesh);

        /**
         * @brief 使用前沿层进法生成棱柱网格
         * 
         * @param surface_mesh 三角形、四边形网格
         * @param args 
         * @param vol_mesh 棱柱网格
         */
        Tiger_API void extrude3d(const SurfaceMesh& surface_mesh, const ExtrudeParameters& args, VolumeMesh& vol_mesh) ;

       


    }

}

#endif