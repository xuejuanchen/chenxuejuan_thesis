
#pragma once
#ifndef _REMESH_PARAMETERS_H_
#define _REMESH_PARAMETERS_H_
#include "parameters.h"
#include <sstream>

namespace TiGER {
class RemeshParameters : public APIPara {
 public:

  std::string toCommandLineString() const {
    std::stringstream ss;
    ss << "--iteration " << iteration_number << " ";
    ss << "-e " << eps_rel << " ";
    ss << "-l " << init_len << " ";
    ss << "--const-size " << const_size << " ";
    ss << "--hmax " << hmax << " ";
    ss << "--hmin " << hmin << " ";
    ss << "--beta " << beta << " ";
    if(b_sizefunction){
      ss << "--size_function "
         << " ";
    }
    ss << "--flen " << feature_len << " ";
    ss << "--fangle " << feature_angle << " ";
    ss << "--split " << split_num << " ";
    ss << "--collapse " << collapse_num << " ";
    ss << "--swap " << swap_num << " ";
    ss << "--smooth " << smooth_num;

    return ss.str();
  };

  // Define setters and getters for each variable
  DEFINE_SETTER(int, iteration_number, RemeshParameters,5);
  DEFINE_SETTER(double, const_size, RemeshParameters,0);
  DEFINE_SETTER(double, eps_rel, RemeshParameters,1e-3);
  DEFINE_SETTER(bool, is_no_intersection, RemeshParameters,false);
  DEFINE_SETTER(bool, quality_angle, RemeshParameters,false);
  DEFINE_SETTER(bool, debug_output, RemeshParameters,false);
  DEFINE_SETTER(bool, use_adaptive_size, RemeshParameters,false);
  DEFINE_SETTER(double, init_len, RemeshParameters,0.01);
  DEFINE_SETTER(double, hmax, RemeshParameters,0);
  DEFINE_SETTER(double, hmin, RemeshParameters,0);
  DEFINE_SETTER(double, beta, RemeshParameters,1.2);
  DEFINE_SETTER(double, feature_len, RemeshParameters,0.01);
  DEFINE_SETTER(double, feature_angle, RemeshParameters,45);
  DEFINE_SETTER(double, curve_angle, RemeshParameters,120.0);
  DEFINE_SETTER(bool, b_nofeature, RemeshParameters,false);
  DEFINE_SETTER(bool, b_no_inter_check, RemeshParameters,false);
  DEFINE_SETTER(bool, b_sizefunction, RemeshParameters,false);
  DEFINE_SETTER(bool, b_adaptive, RemeshParameters,false);
  DEFINE_SETTER(int, split_num, RemeshParameters,1);
  DEFINE_SETTER(int, collapse_num, RemeshParameters,1);
  DEFINE_SETTER(int, swap_num, RemeshParameters,1);
  DEFINE_SETTER(int, smooth_num, RemeshParameters,1);
};
}  // namespace TiGER

#endif  // _DISCRETIZE_CURVE_PARAMETERS_H_