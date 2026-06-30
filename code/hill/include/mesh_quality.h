#pragma once
#ifndef _MESH_QUALITY_H_
#define _MESH_QUALITY_H_
#include <vector>
#include <string>
#include <array>
#include "./mesh_data_struct.h"
#include "./definitions.h"

namespace TiGER {
    struct QualityResult
    {
        double min_value;
        double max_value;
        double ave_value;
        int min_index;
        int max_index;
        int min_index_2; // 用于面积比体积比
        int max_index_2; // 用于面积比体积比
        /* data */
    };

    enum SurfMeshQualityType {
        TRI_ASPECT = 0,
        TRI_AREA,
        TRI_ANGLE,
        TRI_CONDITION,
        TRI_SCALED_JACOBIAN,
        TRI_SHAPE,
        TRI_RELATIVE_SIZE_SQUARED,
        TRI_SHAPE_AND_SIZE,
        TRI_DISTORTION,
        TRI_EDGE_RATIO,
        TRI_SIZE_CHANGE,
        TRI_EQUIANGLE_SKEWNESS,
        TRI_AREA_RATIO
    };
    enum VolMeshQualityType {
        TET_ASPECT_BETA,
        TET_ASPECT_GAMMA,
        TET_VOLUME,
        TET_CONDITION,
        TET_JACOBIAN,
        TET_SCALED_JACOBIAN,
        TET_SHAPE,
        TET_RELATIVE_SIZE,
        TET_SHAPE_AND_SIZE,
        TET_DISTORTION,
        TET_EDGE_RATIO,
        TET_MINIMUM_DIHEDRAL,
        TET_MAXIMUM_DIHEDRAL,
        TET_SIZE_CHANGE,
        TET_EQUIANGLE_SKEWNESS,
        HEX_VOLUME,
        PRISM_VOLUME,
        PRISM_EQUIANGLE_SKEWNESS,
        PRISM_CENTROID_SKEWNESS,
        PYRAMID_VOLUME,
        PYRAMID_EQUIANGLE_SKEWNESS,
        PYRAMID_CENTROID_SKEWNESS,
        VOLUME_RATIO
    };

    /**
     * @brief
     *
     * @param surface_mesh
     * @param type
     * @param quality
     */
    Tiger_API void calculateSurfaceMeshQuality(
        const TiGER::SurfaceMesh& surface_mesh,
        const SurfMeshQualityType& type,
        std::vector<QualityResult>& quality
    );

    /**
     * @brief
     *
     * @param volume_mesh
     * @param type
     * @param quality
     */
    Tiger_API void calculateVolMeshQuality(
        const TiGER::VolumeMesh& volume_mesh,
        const VolMeshQualityType& type,
        std::vector<QualityResult>& quality
    );


};
#endif //!_MESH_QUALITY_H_