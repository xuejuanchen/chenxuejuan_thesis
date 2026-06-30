#include "interface.h"
#include "geometry.hpp"
#include <iostream>
#include <chrono>
#include <cmath>
int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_surface> <input_params> <input_unit>" << std::endl;
        return 1;
    }

    ProjectIO io;

    std::string input_surface = argv[1];
    //std::string input_params = argv[2];
    //std::string input_unit = argv[3];
    size_t dotPos = input_surface.find_last_of(".");
    std::string ext = input_surface.substr(dotPos + 1);
    if (ext == "stl") {
        readSTL(input_surface, io.sf_input);
    }
    else if (ext == "obj") {
        readOBJ(input_surface, io.sf_input);
    }
    else {
        std::cerr << "Error: Unsupported file format (" << input_surface << ")" << std::endl;
        return 1;
    }

    // 读取单元文件
    //std::string unit_file = "D:/mesh/Imprint/model/unit1.vtk";
    //readVTK(unit_file, io.unit_input, "id");
    std::string unit_file = "D:/mesh/Imprint/model/bunnyx30.obj";
    readOBJ(unit_file, io.unit_input);
    io.unit_input.Vertex /= 10;
    Eigen::Matrix3d Rz;
    Rz << 1, 0, 0,
        0, 0, -1,
        0, 1, 0;
    io.unit_input.Vertex = Rz * io.unit_input.Vertex.transpose();
    io.unit_input.Vertex.transposeInPlace();
    //auto start = std::chrono::high_resolution_clock::now(); // 记录起始时间
    writeVTK("unit.vtk", io.unit_input, "id");

    // 调用exe
    std::string model_path = "D:/mesh/Imprint/model/bunnyx30.obj";
    double height = 0.5;
    std::string exe_path = "D:/mesh/Imprint/model/scaf_flow_bin.exe";
    std::string command = exe_path +
        " -m " + model_path +
        " -f " + std::to_string(height) +
        " -n 2 -i 101 -r 1.3 --compress 1e-5 --conj 1";
    int ret_code = std::system(command.c_str());
    io.height = height;

    // 导入生成的三棱柱vtk，存入io.prism
    readVTK("80.vtk", io.prism, "id");

    run(io);
    //auto end = std::chrono::high_resolution_clock::now(); // 记录结束时间
    //std::chrono::duration<double> elapsed = end - start; // 计算时间差
    //std::cout << "Execution Time: " << elapsed.count() << " seconds" << std::endl;
    //std::cout << "Mesh nums: " << hw.output.Topo.rows() << std::endl;


    writeVTK("sf_select.vtk", io.sf_select, "material_id");
    writeVTK("unit_after.vtk", io.unit_input, "id");
    writeVTK("test.vtk", io.output, "id");
    return 1;
}