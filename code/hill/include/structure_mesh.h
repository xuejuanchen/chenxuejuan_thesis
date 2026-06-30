#pragma once
#include <vector>
#include <array>
#include <string>

#include "./geometry_interface.h"
#include "./mesh_data_struct.h"

namespace TiGER{
    namespace structure_mesh{
    /**
     * @brief 扫掠法生成六面体网格（待完善）
     * 
     * @param source_mesh 
     * @param target_mesh 
     * @param args 
     * @param hex_mesh 
     */
    void generateSweepHexMesh(
        const GeometrySurface& source_surface,
        const GeometrySurface& target_surface,
        const SurfaceMesh& side_mesh,
        const std::string& args,
        VolumeMesh& hex_mesh
    );

    /** 
     * @brief 映射法生成六面体网格
     * 
     * */
    void generateMappingHexMesh(
        const VolumeMesh& hex_mesh_in,
        VolumeMesh& hex_mesh_out
    );

    }
}