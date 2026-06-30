#include "geometry_interface_plane.h"
#include <string>
#include <cstdio>
#include <math.h>
#include <algorithm>

using namespace TiGER;

/*
* @brief �������������Ĳ��
*/
static std::array<double, 3> crossProduct(const std::array<double, 3>& a, const std::array<double, 3>& b)
{
    return{
        a[1] * b[2] - a[2] * b[1],
        a[2] * b[0] - a[0] * b[2],
        a[0] * b[1] - a[1] * b[0] };
}

static double calcuteDis(std::array<double, 3> p0, std::array<double, 3> p1)
{
    return
        sqrt((p1[0] - p0[0]) * (p1[0] - p0[0]) +
            (p1[1] - p0[1]) * (p1[1] - p0[1]) +
            (p1[2] - p0[2]) * (p1[2] - p0[2]));
}

static std::function<std::array<double, 3>(const double&)> creatLineFunction(
    std::array<double, 3> p0,
    std::array<double, 3> p1)
{
    std::array<double, 3> basis = std::array<double, 3> {
        (p1[0] - p0[0]), (p1[1] - p0[1]), (p1[2] - p0[2])};
    return[p0, basis](const double u)->std::array<double, 3>
    {
        std::array<double, 3> direction = std::array<double, 3> {u* basis[0], u* basis[1], u* basis[2]};
        std::array<double, 3> res = std::array<double, 3> {p0[0] + direction[0], p0[1] + direction[1], p0[2] + direction[2]};
        return res;
    };
}

static std::function<void(const double&, std::array<double, 3>&)> myLined1Function(
    std::array<double, 3> p0,
    std::array<double, 3> p1)
{
    return[p0, p1](const double& u, std::array<double, 3>& d1)
        {
            d1[0] = p1[0] - p0[0];
            d1[1] = p1[1] - p0[1];
            d1[2] = p1[2] - p0[2];
        };
}

static std::function<void(const double&, std::array<double, 3>&)> myLined2Function()
{
    return[](const double& u, std::array<double, 3>& d2)
        {
            d2 = std::array<double, 3>{0, 0, 0};
        };
}

static std::function<double()> mygetTotalLenthFunction(
    std::array<double, 3> p0,
    std::array<double, 3> p1)
{
    return[p0, p1]()->double
        {
            double length = calcuteDis(p0, p1);
            return length;
        };
}

static std::function<void(const double&, std::array<double, 3>&, double&)> myarcToCoordinateFunction(
    std::array<double, 3> p0,
    std::array<double, 3> p1)
{
    return[p0, p1](const double& arc_length, std::array<double, 3>& xyz, double& u)
        {
            double length = calcuteDis(p0, p1);
            std::array<double, 3> basis = std::array<double, 3>{
                (p1[0] - p0[0]) / length, (p1[1] - p0[1]) / length, (p1[2] - p0[2]) / length};
            xyz = std::array<double, 3>{
                p0[0] + basis[0] * arc_length, p0[1] + basis[1] * arc_length, p0[2] + basis[2] * arc_length};
            u = arc_length / length;
        };
}

static std::function<void(const std::array<double, 3>&, const double&, const double&, double&)> myLineProjectFunction(
    std::array<double, 3> p0,
    std::array<double, 3> p1)
{
    return[p0, p1](const std::array<double, 3>& xyz, const double& eps, const double& hint, double& res_u)
        {
            double length = calcuteDis(p0, p1);
            std::array<double, 3> basis = std::array<double, 3>{
                (p1[0] - p0[0]) / length, (p1[1] - p0[1]) / length, (p1[2] - p0[2]) / length};
            std::array<double, 3> vector_xyz_p0 = std::array<double, 3>{
                xyz[0] - p0[0], xyz[1] - p0[1], xyz[2] - p0[2]};
            double dot_product = basis[0] * vector_xyz_p0[0] + basis[1] * vector_xyz_p0[1] + basis[2] * vector_xyz_p0[2];
            res_u = dot_product / length;
        };
}

static std::function<std::array<double, 3>(std::array<double, 2>)> createPlaneFunction(
    std::array<double, 3> p0, 
    std::array<double, 3> basis1, 
    std::array<double, 3> basis2)
{
    return[basis1, basis2, p0](std::array<double, 2> uv)
        ->std::array<double, 3>{
        std::array<double, 3> result;
        result[0] = basis1[0] * uv[0] + basis2[0] * uv[1] + p0[0];
        result[1] = basis1[1] * uv[0] + basis2[1] * uv[1] + p0[1];
        result[2] = basis1[2] * uv[0] + basis2[2] * uv[1] + p0[2];
        return result;
    };
}

static std::function<void(const std::array<double, 2>&, std::array<double, 3>&, std::array<double, 3>&)> myd1Function(
    std::array<double, 3> basis1,
    std::array<double, 3> basis2)
{
    return[basis1, basis2](const std::array<double, 2>& uv, std::array<double, 3>& du, std::array<double, 3>& dv)
        {
            du = basis1;
            dv = basis2;
        };
}

static std::function<void(const std::array<double, 2>&, std::array<double, 3>&, std::array<double, 3>&, std::array<double, 3>&)> myd2Function()
{
    return[](const std::array<double, 2>&uv, std::array<double, 3>&duu,
        std::array<double, 3>&dvv, std::array<double, 3>&duv)
    {
        duu = std::array<double, 3>{ 0, 0, 0 };
        dvv = std::array<double, 3>{ 0, 0, 0 };
        duv = std::array<double, 3>{ 0, 0, 0 };
    };
}

static std::array<double, 4> mygetUVScaleFunction()
{
    std::array<double, 4> result = { 0, 1, 0, 1 };
    return result;
}

static std::function<void(const std::array<double, 3>&, const double&, const std::array<double, 2>&, std::array<double, 2>&)> creatProjectFunction(
    std::array<double, 3> p0,
    std::array<double, 3> basis1,
    std::array<double, 3> basis2)
{
    return[p0, basis1, basis2](const std::array<double, 3>& xyz, const double& eps, const std::array<double, 2>& hint, std::array<double, 2>& uv)
        {
            // ���������
            double x = xyz[0], y = xyz[1], z = xyz[2];

            // ����ƽ���ڵĵ�p0���ͻ�����u,v
            double ux = basis1[0], uy = basis1[1], uz = basis1[2];
            double vx = basis2[0], vy = basis2[1], vz = basis2[2];
            double p0x = p0[0], p0y = p0[1], p0z = p0[2];

            // ����㵽ƽ�������
            double px = x - p0x, py = y - p0y, pz = z - p0z;

            // ����ƽ�淨����
            std::array<double, 3> normal = crossProduct(basis1, basis2);
            double nx = normal[0], ny = normal[1], nz = normal[2];

            // ����㵽ƽ��ľ���
            double dist = std::abs(px * nx + py * ny + pz * nz) / sqrt(nx * nx + ny * ny + nz * nz);

            // ����ͶӰ��
            double t = (nx * (p0x - x) + ny * (p0y - y) + nz * (p0z - z)) / (nx * nx + ny * ny + nz * nz);
            double proj_x = x + t * nx;
            double proj_y = y + t * ny;
            double proj_z = z + t * nz;

            // ����ͶӰ���ڻ�����u��v��ͶӰ
            double proj_px = proj_x - p0x;
            double proj_py = proj_y - p0y;
            double proj_pz = proj_z - p0z;

            // ����u��vֵ
            double denom = ux * vy - uy * vx;
            uv[0] = (proj_px * vy - proj_py * vx) / denom; // Uֵ
            uv[1] = (proj_py * ux - proj_px * uy) / denom; // Vֵ
        };
} 

void TiGER::creatLine(std::array<double, 3> p0,
    std::array<double, 3> p1,
    GeometryCurve& line)
{
    line.d0Function = creatLineFunction(p0, p1);
    line.d1Function = myLined1Function(p0, p1);
    line.d2Function = myLined2Function();
    line.getTotalLenthFunction = mygetTotalLenthFunction(p0, p1);
    line.projectFunction = myLineProjectFunction(p0, p1);
    line.arcToCoordinateFunction = myarcToCoordinateFunction(p0, p1);
}

void TiGER::creatPlane(std::array<double, 3> p0,
    std::array<double, 3> basis1,
    std::array<double, 3> basis2,
    GeometrySurface& plane)
{
    plane.d0Function = createPlaneFunction(p0, basis1, basis2);
    plane.d1Function = myd1Function(basis1, basis2);
    plane.d2Function = myd2Function();
    plane.getUVScaleFunction = mygetUVScaleFunction;
    plane.projectFunction = creatProjectFunction(p0, basis1, basis2);
}