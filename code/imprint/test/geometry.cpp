#include <vector>
#include <array>
#include <string>
#include <Eigen/Dense>
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>
#include "interface.h"
#include "stl_reader.h"

#ifndef BUFFER_LENGTH
#define BUFFER_LENGTH 256
#endif

static std::vector<std::string> seperate_string(std::string origin) {
    std::vector<std::string> result;
    std::stringstream ss(origin);
    while (ss >> origin) result.push_back(origin);
    return result;
}

int readParams(const std::string& filename, MapParameters& mp) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return -1;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::string param;
        std::istringstream iss(line);

        iss >> param;
        if (param.back() == ':') {
            param.pop_back();
        }

        if (param == "start_pos_uv") {
            iss >> mp.start_pos_uv(0) >> mp.start_pos_uv(1);
        }
        else if (param == "start_pos_xyz") {
            iss >> mp.start_pos(0) >> mp.start_pos(1) >> mp.start_pos(2);
            mp.isUV = false;
        }
        else if (param == "d1_o_uv") {
            iss >> mp.d1_o_uv(0) >> mp.d1_o_uv(1);
        }
        else if (param == "d2_o_uv") {
            iss >> mp.d2_o_uv(0) >> mp.d2_o_uv(1);
        }
        else if (param == "d1_o_xyz") {
            iss >> mp.d1_oritation(0) >> mp.d1_oritation(1) >> mp.d1_oritation(2);
        }
        else if (param == "d2_o_xyz") {
            iss >> mp.d2_oritation(0) >> mp.d2_oritation(1) >> mp.d2_oritation(2);
        }
        else if (param == "d1_distance") {
            iss >> mp.d1_distance;
        }
        else if (param == "d2_distance") {
            iss >> mp.d2_distance;
        }
        else if (param == "height") {
            iss >> mp.height;
        }
    }

    file.close();
    return 0;
}

double convertToDouble(std::string& str) {
    size_t start = str.find_first_not_of(" ");
    if (start != std::string::npos) {
        str.erase(0, start);
    }
    // Check if there's a '-' in the string that represents the scientific notation
    size_t pos = str.find('-');
    int count = std::count(str.begin(), str.end(), '-');

    if (pos != std::string::npos && (pos > 0 || count > 1)) {
        // The '-' sign represents a negative exponent in scientific notation
        std::string base = str.substr(0, pos);  // Extract the base part "4.1735"
        std::string exponent = str.substr(pos); // Extract the exponent part "-2"

        // Combine the base with the exponent in scientific notation format
        std::stringstream ss;
        ss << base << "e" << exponent; // "4.1735e-2"

        double result;
        ss >> result;

        return result;
    }
    else {
        // If there's no exponent, simply convert the string to double
        double res = std::stod(str);
        return res;
    }
}

int readNAS(const std::string& filename, Mesh& mesh) {
    std::ifstream infile(filename);
    std::string line;
    std::vector<Eigen::RowVector3d> vertices;
    std::vector<Eigen::RowVector3i> topologies;

    while (std::getline(infile, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;

        // Ignore comment lines or empty lines
        if (line.empty() || line[0] == '$') {
            continue;
        }

        // Check for GRID card
        if (line.substr(0, 4) == "GRID") {
            //std::istringstream iss(line.substr(8)); // Start after "GRID"

            std::string id_str = line.substr(8, 8);
            std::string x_str = line.substr(24, 8);
            std::string y_str = line.substr(32, 8);
            std::string z_str = line.substr(40, 8);

            int id = std::stoi(id_str);
            double x = convertToDouble(x_str);
            double y = convertToDouble(y_str);
            double z = convertToDouble(z_str);

            // Add the point to the vertices vector
            vertices.push_back(Eigen::RowVector3d(x, y, z));
        }
        else if (keyword == "CTRIA3") {
            int elementId, propId, n1, n2, n3;
            iss >> elementId >> propId >> n1 >> n2 >> n3;
            topologies.push_back(Eigen::RowVector3i(n1 - 1, n2 - 1, n3 - 1)); // Convert 1-based to 0-based index
            std::vector<int> temp;
            temp.push_back(propId);
            mesh.attribute.push_back(temp);
        }
    }

    // Convert vector to Eigen matrices
    mesh.Vertex.resize(vertices.size(), 3);
    for (size_t i = 0; i < vertices.size(); ++i) {
        mesh.Vertex.row(i) = vertices[i];
    }

    mesh.Topo.resize(topologies.size(), 3);
    for (size_t i = 0; i < topologies.size(); ++i) {
        mesh.Topo.row(i) = topologies[i];
    }

    return 1;
}

int readNAS_newversion(const std::string& filename, Mesh& mesh){
    std::ifstream file(filename);
    std::string line;

    std::vector<Eigen::RowVector3d> points;  // 存储点的坐标
    std::vector<std::vector<int>> topologyData;  // 存储拓扑数据
    std::vector<std::vector<int> > attributesData;  // 存储属性数据

    // 逐行读取文件
    while (std::getline(file, line)) {
        // 处理 GRID* 数据行
        if (line.find("GRID*") != std::string::npos) {
            std::istringstream iss(line);
            int id;
            double x, y;
            int temp;
            // 读取 GRID* 行中的数据
            iss >> temp >> id >> x >> y >> temp;

            // 读取第二行的数据，提取 Z 坐标
            std::getline(file, line);
            std::istringstream iss2(line);
            double z;
            // 读取第二行中的 Z 坐标
            iss2 >> temp;
            iss2 >> z;

            // 将数据存储到 points 中
            Eigen::RowVector3d point(x, y, z);
            points.push_back(point);
        }
        // 处理 CTRIA3 数据行
        else if (line.find("CTRIA3") != std::string::npos) {
            std::istringstream iss(line);
            std::string elementType;
            int elementID, materialID, node1, node2, node3;
            // 读取 CTRIA3 行的数据
            iss >> elementType >> elementID >> materialID >> node1 >> node2 >> node3;

            // 将拓扑数据添加到 topologyData 中
            std::vector<int> topology = { node1 - 1, node2 - 1, node3 - 1 };
            topologyData.push_back(topology);
            std::vector<int> temp;
            temp.push_back(materialID);
            attributesData.push_back(temp);  // 存储材料 ID 或其他属性信息
        }
    }

    // 将点坐标填充到 Eigen::MatrixXd Vertex 中
    int numPoints = points.size();
    mesh.Vertex.resize(numPoints, 3);
    for (int i = 0; i < numPoints; ++i) {
        mesh.Vertex.row(i) = points[i];
    }

    // 将拓扑数据填充到 Eigen::MatrixXi Topology 中
    int numElements = topologyData.size();
    mesh.Topo.resize(numElements, 3);
    for (int i = 0; i < numElements; ++i) {
        mesh.Topo.row(i) = Eigen::Vector3i(topologyData[i][0], topologyData[i][1], topologyData[i][2]);
    }

    // 将属性数据填充到 Attributes 中
    mesh.attribute = attributesData;
    return 0;
}

int readSTL(std::string filename, Mesh& mesh) {
    auto& M = mesh.Masks;
    auto& V = mesh.Vertex;
    auto& T = mesh.Topo;
    int nPoints = 0;
    int nFacets = 0;

    stl_reader::StlMesh<> stlmesh;
    int result = stlmesh.read_file(filename);
    if (!result) {
        return -1;
    }

    nPoints = stlmesh.num_vrts(); // 点数量
    V.resize(nPoints, 3);

    for (int i = 0; i < nPoints; i++) {
        V.row(i) << stlmesh.raw_coords()[i * 3 + 0], stlmesh.raw_coords()[i * 3 + 1], stlmesh.raw_coords()[i * 3 + 2];
    }

    nFacets = stlmesh.num_tris(); // 三角面片数量
    T.resize(nFacets, 3);
    for (int i = 0; i < nFacets; i++) {
        for (int j = 0; j < 3; j++) {
            T(i, j) = stlmesh.tri_corner_ind(i, j);
        }
    }

    M.resize(nFacets, 1);  // 面id
    Eigen::RowVector3d normal;
    normal << stlmesh.tri_normal(0)[0], stlmesh.tri_normal(0)[1], stlmesh.tri_normal(0)[2];
    int face_id = 0;
    for (int i = 0; i < nFacets; i++) {
        //Eigen::RowVector3d temp;
        //temp << stlmesh.tri_normal(i)[0], stlmesh.tri_normal(i)[1], stlmesh.tri_normal(i)[2];
        //if (temp != normal) {
        //    face_id++;
        //    normal = temp;
        //}
        M(i, 0) = face_id;
    }

    return 1;
}
int writeVTK(std::string filename, const Mesh& mesh, std::string mark_pattern) {
    auto& M = mesh.masks;
    auto& V = mesh.Vertex;
    auto& T = mesh.Topo;
    FILE* f = fopen(filename.c_str(), "w");
    if (!f) {
        std::cout << "Write VTK file failed. - " << filename << std::endl;
        return -1;
    }
    std::cout << "Writing mesh to - " << filename << std::endl;
    fprintf(f, "# vtk DataFile Version 2.0\n");
    fprintf(f, "TetWild Mesh\n");
    fprintf(f, "ASCII\n");
    fprintf(f, "DATASET UNSTRUCTURED_GRID\n");
    fprintf(f, "POINTS %d double\n", V.rows());
    for (int i = 0; i < V.rows(); i++) {
        fprintf(f, "%.17g %.17g %.17g\n", V(i, 0), V(i, 1), V(i, 2));
    }
    fprintf(f, "CELLS %d %d\n", T.rows(), T.rows() * (T.cols() + 1));
    for (int i = 0; i < T.rows(); i++) {
        fprintf(f, "%d ", T.cols());
        for (int j = 0; j < T.cols(); j++) {
            fprintf(f, "%d ", T(i, j));
        }
        fprintf(f, "\n");
    }
    fprintf(f, "CELL_TYPES %d\n", T.rows());
    int cellType = 0;
    if (T.cols() == 2) {
        cellType = 3;
    }
    else if (T.cols() == 3) {
        cellType = 5;
    }
    else if (T.cols() == 4) {
        cellType = 9;
    }
    else if (T.cols() == 6) {
        cellType = 13;
    }
    else if (T.cols() == 8) {
        cellType = 12;
    }

    for (int i = 0; i < T.rows(); i++) {
        fprintf(f, "%d\n", cellType);
    }
    if (M.size() != T.rows()) {
        fclose(f);
        return 1;
    }
    fprintf(f, "CELL_DATA %d\n", T.rows());
    fprintf(f, "SCALARS cell_id int 1\n");
    fprintf(f, "LOOKUP_TABLE default\n");
    for (int i = 0; i < T.rows(); i++) {
        fprintf(f, "%d\n", M[i][0]);
    }
    //fprintf(f, "SCALARS cell_id_2 int 1\n");
    //fprintf(f, "LOOKUP_TABLE default\n");
    //for (int i = 0; i < T.rows(); i++) {
    //    fprintf(f, "%d\n", M[i].size()-1? M[i][1]:-1);
    //}
    //fprintf(f, "SCALARS unit_attribute int 1\n");
    //fprintf(f, "LOOKUP_TABLE default\n");
    //for (int i = 0; i < T.rows(); i++) {
    //    fprintf(f, "%d\n", mesh.attribute[i][0]);
    //}
    //fprintf(f, "SCALARS unit_attribute_2 int 1\n");
    //fprintf(f, "LOOKUP_TABLE default\n");
    //for (int i = 0; i < T.rows(); i++) {
    //    fprintf(f, "%d\n", mesh.attribute[i].size()>1? mesh.attribute[i][1]:-1);
    //}

    fclose(f);
    return 1;
}

int writeVTK_grid(int Im, int Jm, const std::vector<Eigen::RowVector2d>& vertices, const std::string& filename)
{
    std::ofstream vtkFile(filename.c_str());

    if (vtkFile.is_open())
    {
        // write header
        vtkFile << "# vtk DataFile Version 2.0\n";
        vtkFile << "Structured Grid\n";
        vtkFile << "ASCII\n";
        vtkFile << "DATASET STRUCTURED_GRID\n";

        // write vertices
        vtkFile << "DIMENSIONS " << Im << " " << Jm << " 2\n";
        vtkFile << "POINTS " << vertices.size() << " double\n";
        for (auto vt : vertices)
        {
            vtkFile << vt(0) << " " << vt(1) << " " << 0 << "\n";
        }

        vtkFile.close();
        std::cout << "Write structured_grid file finished." << std::endl;
    }
    else
    {
        std::cout << "can't open the file." << std::endl;
    }

    return 0;
}

int writeVTK_uv(std::string filename, const Mesh& mesh, std::string mark_pattern) {
    auto& V = mesh.V_uv;
    auto& T = mesh.T_uv;
    auto& M = mesh.Masks;
    FILE* f = fopen(filename.c_str(), "w");
    if (!f) {
        std::cout << "Write VTK file failed. - " << filename << std::endl;
        return -1;
    }
    std::cout << "Writing mesh to - " << filename << std::endl;
    fprintf(f, "# vtk DataFile Version 2.0\n");
    fprintf(f, "TetWild Mesh\n");
    fprintf(f, "ASCII\n");
    fprintf(f, "DATASET UNSTRUCTURED_GRID\n");
    fprintf(f, "POINTS %d double\n", V.rows());
    for (int i = 0; i < V.rows(); i++) {
        fprintf(f, "%.17g %.17g %.17g\n", V(i, 0), V(i, 1), 0);
    }
    fprintf(f, "CELLS %d %d\n", T.rows(), T.rows() * (T.cols() + 1));
    for (int i = 0; i < T.rows(); i++) {
        fprintf(f, "%d ", T.cols());
        for (int j = 0; j < T.cols(); j++) {
            fprintf(f, "%d ", T(i, j));
        }
        fprintf(f, "\n");
    }
    fprintf(f, "CELL_TYPES %d\n", T.rows());
    int cellType = 0;
    if (T.cols() == 2) {
        cellType = 3;
    }
    else if (T.cols() == 3) {
        cellType = 5;
    }
    else if (T.cols() == 4) {
        cellType = 10;
    }
    else if (T.cols() == 6) {
        cellType = 13;
    }

    for (int i = 0; i < T.rows(); i++) {
        fprintf(f, "%d\n", cellType);
    }
    if (M.rows() != T.rows()) {
        fclose(f);
        return 1;
    }
    //fprintf(f, "POINT_DATA %d\n", V.rows());
    //fprintf(f, "SCALARS point_id int 1\n");
    //fprintf(f, "LOOKUP_TABLE default\n");
    //for (int i = 0; i < V.rows(); i++) {
    //    fprintf(f, "%d\n", i);
    //}
    // fprintf(f, "CELL_DATA %d\n", T.rows());
    // fprintf(f, "SCALARS %s int %d\n", mark_pattern.c_str(), 1);
    // fprintf(f, "LOOKUP_TABLE default\n");
    // for (int i = 0; i < T.rows(); i++) {
    //         fprintf(f, "%d", i);
    //     fprintf(f, "\n");
    // }
    //fprintf(f, "CELL_DATA %d\n", M.rows());
    //fprintf(f, "SCALARS %s int %d\n", mark_pattern.c_str(), M.cols());
    //fprintf(f, "LOOKUP_TABLE default\n");
    //for (int i = 0; i < M.rows(); i++) {
    //    for (int j = 0; j < M.cols(); j++) {
    //        fprintf(f, "%d", M(i, j));
    //    }
    //    fprintf(f, "\n");
    //}
    //if (T.cols() == 3) {
    //    std::cout << "This have normal information.\n";
    //    fprintf(f, "NORMALS facet_normals double\n");
    //    for (int i = 0; i < T.rows(); i++) {
    //        Eigen::Vector3d ab = V.row(T(i, 1)) - V.row(T(i, 0));
    //        Eigen::Vector3d ac = V.row(T(i, 2)) - V.row(T(i, 0));
    //        Eigen::Vector3d t_nor = ab.cross(ac);
    //        t_nor.normalize();
    //        fprintf(f, "%.17g %.17g %.17g\n", t_nor.x(), t_nor.y(), t_nor.z());
    //    }
    //}
    fclose(f);
    return 1;
}

int readOBJ(std::string filename, Mesh& mesh) {
    auto& M = mesh.Masks;
    auto& V = mesh.Vertex;
    auto& T = mesh.Topo;
    std::ifstream objFile;
    objFile.open(filename);
    if (!objFile.is_open()) {
        std::cout << "No such file. - " << filename << std::endl;
        return -1;
    }
    char buffer[256];
    std::vector<std::vector<double>> plist;
    std::vector<std::vector<int>> flist;
    std::vector<int> mlist;
    int curMark = 0;
    while (!objFile.eof()) {
        objFile.getline(buffer, 256);
        if (buffer[0] == 'v') {
            std::vector<std::string> words = seperate_string(std::string(buffer));
            if (words.size() < 4) {
                continue;
            }
            std::vector<double> coord;
            for (int ii = 1; ii < 4; ii++) {
                int wordLen = 0;
                while (wordLen < words[ii].length()) {
                    if (words[ii][wordLen] == '/') {
                        break;
                    }
                    wordLen++;
                }
                coord.push_back(stod(words[ii].substr(0, wordLen)));
            }
            plist.push_back(coord);
        }
        if (buffer[0] == 'f') {
            std::vector<std::string> words = seperate_string(std::string(buffer));
            if (words.size() < 4) {
                continue;
            }
            std::vector<int> facet;
            for (int ii = 1; ii < 4; ii++) {
                int wordLen = 0;
                while (wordLen < words[ii].length()) {
                    if (words[ii][wordLen] == '/') {
                        break;
                    }
                    wordLen++;
                }
                facet.push_back(stoi(words[ii].substr(0, wordLen)) - 1);
            }
            flist.push_back(facet);
            mlist.push_back(curMark);
        }
        if (buffer[0] == 'g') {
            ++curMark;
        }
    }

    V.resize(plist.size(), 3);
    for (int i = 0; i < plist.size(); i++) {
        for (int j = 0; j < 3; j++) {
            V(i, j) = plist[i][j];
        }
    }
    T.resize(flist.size(), 3);
    for (int i = 0; i < flist.size(); i++) {
        for (int j = 0; j < 3; j++) {
            T(i, j) = flist[i][j];
        }
    }
    M.resize(mlist.size(), 1);
    for (int i = 0; i < mlist.size(); i++) {
        M(i, 0) = mlist[i];
    }

    return 1;
}

int readVTK(std::string filename, Mesh& mesh, std::string mark_pattern)
{
    auto readVTK_newer_version = [](std::string filename, Mesh& mesh, std::string mark_pattern) -> int {
        auto& M = mesh.Masks;
        auto& V = mesh.Vertex;
        auto& T = mesh.Topo;
        int nPoints = 0;
        int nFacets = 0;
        std::ifstream vtk_file;
        vtk_file.open(filename);
        if (!vtk_file.is_open())
        {
            std::cout << "No such file. - " << filename << std::endl;
            return -1;
        }
        std::string vtk_type_str = "CONNECTIVITY";
        char buffer[BUFFER_LENGTH];
        while (!vtk_file.eof())
        {
            vtk_file.getline(buffer, BUFFER_LENGTH);
            std::string line = (std::string)buffer;
            if (vtk_file.peek() == EOF)
                break;
            if (line.length() < 2 || buffer[0] == '#')
                continue;

            if (line.find("POINTS ") != std::string::npos)
            {
                std::vector<std::string> words = seperate_string(line);
                nPoints = stoi(words[1]);
                V.resize(nPoints, 3);
                for (int i = 0; i < nPoints; i++)
                {
                    vtk_file >> V(i, 0) >> V(i, 1) >> V(i, 2);
                }
            }
            if (line.find("CELLS") != std::string::npos)
            {
                std::vector<std::string> words = seperate_string(line);
                nFacets = stoi(words[1]) - 1;
                T.resize(nFacets, 3);
            }
            if (line.find("CONNECTIVITY") != std::string::npos)
            {
                std::string trash;
                // vtk_file >> trash;
                for (int i = 0; i < nFacets; i++)
                {
                    vtk_file >> T(i, 0) >> T(i, 1) >> T(i, 2);
                }
                //  std::cout << T(0, 0) << std::endl;
            }
        }
        //  std::cout << T(0, 0) << std::endl;
        vtk_file.close();
        return 1;
        };

    auto& M = mesh.Masks;
    auto& V = mesh.Vertex;
    auto& T = mesh.Topo;
    int nPoints = 0;
    int nFacets = 0;
    std::ifstream vtk_file;
    vtk_file.open(filename);

    if (!vtk_file.is_open())
    {
        std::cerr << "无法打开文件: " << filename << "，错误: " << std::strerror(errno) << std::endl;
        // std::cout << "No such file. - " << filename << std::endl;
        return -1;
    }
    std::string vtk_type_str = "POLYDATA ";
    char buffer[BUFFER_LENGTH];
    while (!vtk_file.eof())
    {
        vtk_file.getline(buffer, BUFFER_LENGTH);
        std::string line = (std::string)buffer;
        if (line.length() < 2)
            continue;
        if (line.find("Version") != std::string::npos)
        {
            std::vector<std::string> words = seperate_string(line);
            if (words[4][0] == '5')
            {
                vtk_file.close();
                return readVTK_newer_version(filename, mesh, mark_pattern);
            }
        }
        if (line.find("DATASET") != std::string::npos)
        {
            std::vector<std::string> words = seperate_string(line);
            if (words[1] == "POLYDATA")
                vtk_type_str = "POLYGONS ";
            else if (words[1] == "UNSTRUCTURED_GRID")
                vtk_type_str = "CELLS ";
            else
            {
                std::cout << "The format of VTK file is illegal, No clear DATASET name. - " << filename << std::endl;
            }
        }
        if (line.find("POINTS ") != std::string::npos)
        {
            std::vector<std::string> words = seperate_string(line);
            nPoints = stoi(words[1]);
            V.resize(nPoints, 3);
            for (int i = 0; i < nPoints; i++)
            {
                vtk_file.getline(buffer, BUFFER_LENGTH);
                words = seperate_string(std::string(buffer));
                for (int j = 0; j < 3; ++j)
                {
                    V(i, j) = stod(words[j]);
                }
            }
        }
        if (line.find(vtk_type_str) != std::string::npos)
        {
            std::vector<std::string> words = seperate_string(line);
            nFacets = stoi(words[1]);
            T.resize(nFacets, stoi(words[2]) / nFacets - 1);
            for (int i = 0; i < nFacets; i++)
            {
                vtk_file.getline(buffer, BUFFER_LENGTH);
                words = seperate_string(std::string(buffer));
                for (int j = 0; j < stoi(words[0]); j++)
                    T(i, j) = stoi(words[j + 1]);
            }
            M.resize(nFacets, 1);
            for (int i = 0; i < nFacets; i++)
                M(i, 0) = 0;
        }
        if (line.find("CELL_DATA ") != std::string::npos)
        {
            std::vector<std::string> words = seperate_string(line);
            if (stoi(words[1]) != nFacets)
            {
                std::cout << "The number of CELL_DATA is not equal to number of cells. -" << filename;
                std::cout << "Ignore CELL_DATA" << std::endl;
                return 0;
            }
            if (vtk_type_str == "POLYGONS ")
            {
                vtk_file.getline(buffer, BUFFER_LENGTH);
                vtk_file.getline(buffer, BUFFER_LENGTH);
                std::string data_type = seperate_string(std::string(buffer))[0];
                if (data_type != mark_pattern)
                    continue;
                for (int i = 0; i < nFacets; i++)
                {
                    vtk_file.getline(buffer, BUFFER_LENGTH);
                    int surface_id = stoi(std::string(buffer));
                    M(i, 0) = surface_id;
                }
            }
            else
            {
                vtk_file.getline(buffer, BUFFER_LENGTH);
                std::string data_type = seperate_string(std::string(buffer))[1];
                if (data_type != mark_pattern)
                    continue;
                vtk_file.getline(buffer, BUFFER_LENGTH);
                for (int i = 0; i < nFacets; i++)
                {
                    vtk_file.getline(buffer, BUFFER_LENGTH);
                    int surface_id = stoi(std::string(buffer));
                    M(i, 0) = surface_id;
                }
            }
        }
    }
    vtk_file.close();
    return 1;
}