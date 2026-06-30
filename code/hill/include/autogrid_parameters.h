
#pragma once
#ifndef _DISCRETIZE_AUTOGRID_H_
#define _DISCRETIZE_AUTOGRID_H_
#include "parameters.h"
#include <sstream>

namespace TiGER {
class AutogridParameters : public APIPara {
 public:

  std::string toCommandLineString() const {
    std::stringstream ss;
    ss << "--max-pass " << max_num_passes << " ";
    if (use_local) ss << "--local ";
    ss << "-e " << eps_rel;
    return ss.str();
  };

  // Define setters and getters for each variable
  DEFINE_SETTER(int, max_num_passes, AutogridParameters,5);
  DEFINE_SETTER(bool, use_local, AutogridParameters, false);
  DEFINE_SETTER(double, eps_rel, AutogridParameters,1e-3);

};
}  // namespace TiGER

#endif  // _DISCRETIZE_CURVE_PARAMETERS_H_
