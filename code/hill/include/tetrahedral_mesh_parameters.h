#pragma once
#ifndef _TETRAHEDRAL_PARAMETERS_H_
#define _TETRAHEDRAL_PARAMETERS_H_
#include "./parameters.h"
namespace TiGER
{
	class TetrahedraParameters : public APIPara
	{

		DEFINE_SETTER(int, infolevel, TetrahedraParameters,1);
		DEFINE_SETTER(int, constrain, TetrahedraParameters,1);
		DEFINE_SETTER(int, refine, TetrahedraParameters,1);
		DEFINE_SETTER(int, optlevel, TetrahedraParameters,3);
		DEFINE_SETTER(int, optloop, TetrahedraParameters,6);
		DEFINE_SETTER(int, onlyopt, TetrahedraParameters,0);
		DEFINE_SETTER(int, solvedomain, TetrahedraParameters,0);
		DEFINE_SETTER(int, nthread, TetrahedraParameters,-1);
		DEFINE_SETTER(double, size, TetrahedraParameters,1);
		DEFINE_SETTER(double, decay, TetrahedraParameters,-1);
		DEFINE_SETTER(double, minEdge, TetrahedraParameters,-1);
		DEFINE_SETTER(double, maxEdge, TetrahedraParameters,-1);
		DEFINE_SETTER(double, minSize, TetrahedraParameters,-1);
		DEFINE_SETTER(double, maxSize, TetrahedraParameters,-1);
		DEFINE_SETTER(double, backgroundSpace, TetrahedraParameters,-1);
		DEFINE_SETTER(double, EdgeGrowthRate, TetrahedraParameters,-1);
		DEFINE_SETTER(double, optangle, TetrahedraParameters,30);
		DEFINE_SETTER(double, optratio, TetrahedraParameters,0.3);
		DEFINE_SETTER(std::vector<double>, hole, TetrahedraParameters,{});

		// int infolevel;				//输出信息等级，设置为0输出最少，作为库，建议设置为0
		// int constrain;				//0，conforming;1,constrain
		// int refine;			    	//是否开启网格细化，默认打开
		// int optlevel;		    	//优化等级，0~4，0不优化，默认最高4，质量最慢，实测3更好插点
		// int optloop;			    //优化轮数，默认8
		// int onlyopt;		    	//是否仅优化，单独优化体网格，默认关闭
		// int solvedomain;			//区域求解，不完善，默认0
		// int nthread;			    //并行优化线程数，可以主动设置，不设置会默认根据网格量自动设置线程数
		// double size;		    	//网格过渡尺寸，默认1，越小越密
		// double decay;				//-1,don't use;0-1
		// double minEdge;				//最小网格边长，默认不启动（-1），可以主动设置
		// double maxEdge;				//最大网格边长，默认不启动（-1），可以主动设置
		// double minSize;				//最小网格体积，默认不启动（-1），可以主动设置
		// double maxSize;				//最大网格体积，默认不启动（-1），可以主动设置
		// double backgroundSpace;		//默认背景尺寸，默认不启动（-1），可以主动设置
		// double EdgeGrowthRate;		//边界增长率，默认不启动（-1），大于1
		// double optangle;			//优化目标角度，默认20°，推荐设置小于30°
		// double optratio;			//优化体积边长比目标，默认0.3，推荐小于0.4
		// std::vector<double> hole;	//挖洞的坐标，可以是连续的x1,y1,z1;x2,y2,z2;... ...
	};
}

#endif // _EXTRUDE_PARAMETERS_H_