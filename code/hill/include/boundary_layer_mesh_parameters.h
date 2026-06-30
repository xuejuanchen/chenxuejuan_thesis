
#pragma once
#ifndef _BOUNDARY_LAYER_PARAMETERS_H_
#define _BOUNDARY_LAYER_PARAMETERS_H_
#include "./parameters.h"
namespace TiGER {
struct BoundaryLayerParameters : public APIPara {
  DEFINE_SETTER(bool, multiple_normal, BoundaryLayerParameters,false);
  DEFINE_SETTER(double, step_length, BoundaryLayerParameters,0.01);
  DEFINE_SETTER(double, ratio, BoundaryLayerParameters,1.2);
  DEFINE_SETTER(double, aniso_iso_blend, BoundaryLayerParameters,1.0);
  DEFINE_SETTER(double, isotropic_height, BoundaryLayerParameters,1.0);
  DEFINE_SETTER(double, delay_skew_criteria, BoundaryLayerParameters,1.0);
  DEFINE_SETTER(double, max_angle, BoundaryLayerParameters,180);
  DEFINE_SETTER(double, equi_volume, BoundaryLayerParameters,1);
  DEFINE_SETTER(double, equi_angle, BoundaryLayerParameters,1);
  DEFINE_SETTER(double, centroid, BoundaryLayerParameters,1);
  DEFINE_SETTER(int, number_of_layer, BoundaryLayerParameters,10);
  DEFINE_SETTER(int, isotropic_layer, BoundaryLayerParameters,0);
  DEFINE_SETTER(int, collision_buffer, BoundaryLayerParameters,1.0);
  DEFINE_SETTER(int, smooth_times, BoundaryLayerParameters,20);
};
}  // namespace TiGER

#endif  // _DISCRETIZE_CURVE_PARAMETERS_H_