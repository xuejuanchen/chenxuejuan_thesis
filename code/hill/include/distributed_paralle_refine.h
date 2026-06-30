#include "mesh_data_struct.h"

namespace TiGER
{
    /**
     * @brief 并行细分网格生成（待完善） 
     * 
     * @param volmesh_in 
     * @param surfmesh 
     * @param args 
     * @param volmesh_out 
     */
    void distributeParalleRefine(
        const VolumeMesh& volmesh_in,
        const SurfaceMesh& surfmesh,
        const std::string args,
        ParallelVolumeMesh& volmesh_out
    );
} // namespace TiGER
