#include <cmath>
#include <stdexcept>
#include <vector>
#include "mesh_data_struct.h"

struct point {
  double x, y, z;
};

void twoPointsToLineMesh(point p1, point p2, TiGER::LineMesh &linemesh,
                         int intervals, int sf_id, std::array<double, 3> sf_normal) 
{
  if (intervals <= 0) 
  {
    throw std::invalid_argument("Intervals must be greater than zero.");
  }
  // Calculate the total length of the line segment
  double totalLength =
      sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2) + pow(p2.z - p1.z, 2));

  // Calculate the direction vector of the line segment
  double dx = (p2.x - p1.x) / totalLength;
  double dy = (p2.y - p1.y) / totalLength;
  double dz = (p2.z - p1.z) / totalLength;

  // Calculate the interval length
  double intervalLength = totalLength / intervals;

  // Generate points along the line segment with equal intervals
  std::array<double, 3> p_tangent = { (p2.x - p1.x) / totalLength, (p2.y - p1.y) / totalLength, (p2.z - p1.z) / totalLength };
  std::array<double, 3> p_normal = {
    sf_normal[1] * p_tangent[2] - sf_normal[2] * p_tangent[1],
    sf_normal[2] * p_tangent[0] - sf_normal[0] * p_tangent[2],
    sf_normal[0] * p_tangent[1] - sf_normal[1] * p_tangent[0] };
  for (int i = 0; i <= intervals; ++i) {
    point newPoint;
    newPoint.x = p1.x + i * intervalLength * dx;
    newPoint.y = p1.y + i * intervalLength * dy;
    newPoint.z = p1.z + i * intervalLength * dz;
    std::array<double, 3> point = { newPoint.x, newPoint.y, newPoint.z };
    linemesh.coord.push_back(point);
    linemesh.point_normal.push_back(p_normal);
    if(i != intervals)
    {
        std::array<int, 2> segment = { i, i + 1 };
        linemesh.segments.push_back(segment);
        linemesh.regions.push_back(sf_id);
    }
  }
  //calculate point normal

}
