#ifndef _TIGER_PARAMETERS_H_
#define _TIGER_PARAMETERS_H_
#include "./discretize_curve_parameters.h"
#include "./extrude_parameters.h"
#include "./discretize_curve_parameters.h"
#include "./discretize_surface_delaunay_parameters.h"
#include "./discretize_surface_remesh_parameters.h"
#include "./sizing_field_parameters.h"
#include "./boundary_layer_mesh_parameters.h"
#include "./tetrahedral_mesh_parameters.h"

namespace TiGER{
    class Parameters:
    virtual public ExtrudeParameters,
    virtual public BoundaryLayerParameters,
    virtual public TetrahedraParameters,
    virtual public CurveParametersDimension,
    virtual public IsotropicSurfaceParametersTri,
    virtual public BackGroundParameters
    {
        

    };
}

#endif //！_TIGER_PARAMETERS_H_