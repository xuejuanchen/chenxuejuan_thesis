#pragma once
#ifndef _TETRAHEDRAL_PARAMETERS_H_
#define _TETRAHEDR_PARAMETERS_H_
#include "./parameters.h"
namespace TiGER
{
	class TriangleDealunay : public APIPara
	{
		DEFINE_SETTER(bool, addinnerpoint, TriangleDealunay,true);//并行优化线程数，可以主动设置，不设置会默认根据网格量自动设置线程数
		DEFINE_SETTER(double, sizealpha, TriangleDealunay,0.7);//网格过渡尺寸，默认1，越小越密
		    	
	};
}

#endif // _EXTRUDE_PARAMETERS_H_