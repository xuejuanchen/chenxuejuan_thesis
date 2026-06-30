#ifndef TEST_LINEMESH_HPP
#define TEST_LINEMESH_HPP

#include <string>
#include <vector>

namespace TiGER {
namespace test_linemesh {
int testLineMesh(std::string filename,
                 std::vector<std::array<double, 3>> linemesh) {
  ifstream pt_wise;
  pt_wise.open(filename);
  if (!pt_wise.is_open()) {
    std::cout << "No such file. - " << filename << std::endl;
    return -1;
  }
  assert(linemesh.size() > 1);
  int pt_num;
  std::vector<std::array<double, 3>> line_in;
  char buffer[BUFFER_LENGTH];

  pt_wise >> buffer;
  pt_num = stoi(buffer);

  if (pt_num != linemesh.size()) {
    std::cout << "number of points dont match\n";
    // return -1;
  }

  for (int i = 0; i < pt_num; i++) {
    std::array<double, 3> pt;
    for (int j = 0; j < 3; j++) {
      pt_wise >> buffer;
      pt[j] = stod(buffer);
    }
    line_in.push_back(pt);
  }
  pt_wise.close();
  double ttl_in = 0.;
  double ttl = 0.;
  double max_diff = 0.;
  double ave_diff = 0.;
  std::vector<double> segement;
  std::vector<double> segement_in;
  for (int i = 0; i < line_in.size() - 1; i++) {
    double len;
    len = sqrt((line_in[i + 1][0] - line_in[i][0]) *
                   (line_in[i + 1][0] - line_in[i][0]) +
               (line_in[i + 1][1] - line_in[i][1]) *
                   (line_in[i + 1][1] - line_in[i][1]) +
               (line_in[i + 1][2] - line_in[i][2]) *
                   (line_in[i + 1][2] - line_in[i][2]));
    segement_in.push_back(len);
    ttl_in += len;
  }
  for (int i = 0; i < linemesh.size() - 1; i++) {
    double len;
    len = sqrt((linemesh[i + 1][0] - linemesh[i][0]) *
                   (linemesh[i + 1][0] - linemesh[i][0]) +
               (linemesh[i + 1][1] - linemesh[i][1]) *
                   (linemesh[i + 1][1] - linemesh[i][1]) +
               (linemesh[i + 1][2] - linemesh[i][2]) *
                   (linemesh[i + 1][2] - linemesh[i][2]));
    segement.push_back(len);
    ttl += len;
  }

  std::cout << "total lenth diff " << (ttl - ttl_in) / ttl * 100 << "%\n";
  for (int i = 0; i < segement.size(); i++) {
    double lenth_diff = segement[i] - segement_in[i];
    std::cout << "single lenth diff   " << lenth_diff << "  "
              << fabs(lenth_diff) / segement_in[i] * 100 << "% \n";
    max_diff = std::max(fabs(lenth_diff) / segement_in[i] * 100, max_diff);
    ave_diff += fabs(lenth_diff) / segement[i] * 100;
  }
  std::cout << "max diff " << max_diff << "%\n";
  std::cout << "ave diff " << ave_diff / segement.size() << "%\n";

  return 1;
}
}  // namespace test_linemesh
}  // namespace TiGER
#endif