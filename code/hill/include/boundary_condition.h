#include "./mesh_data_struct.h"
namespace TiGER{
namespace BoundaryCondition{
enum BCType{
    wall=0,
    far_field,
    unspecified,
    degenerate_line,
    degenerate_point,
    dirichlet,
    extrapolate,
    general,
    inflow,
    inflow_subsonic,
    inflow_supersonic,
    inflow_neumann,
    outflow,
    outflow_subsonic,
    outflow_supersonic,
    symmetry_plane,
    symmetry_polar,
    tunnel_inflow,
    wall_inviscid,
    wall_viscous,
    wall_viscous_heat_flux
}

void setBoundaryCondition();


}
}