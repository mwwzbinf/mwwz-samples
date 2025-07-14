#pragma once
#include"apiExport.h"

#ifdef MWWZ_CPLUSPLUS
namespace mwwz
{
#endif

#ifdef __cplusplus           
extern "C" {
#endif

	struct RRect
	{
		float x;
		float y;
		float angle;//deg
		float len1;
		float len2;
		RRect() : x(0), y(0), angle(0), len1(0), len2(0) {}
		RRect(float _x, float _y, float _a, float _len1, float _len2) :x(_x), y(_y), angle(_a), len1(_len1), len2(_len2) {}
	};

	struct ParaFit
	{
		int mode;//0 直线 1 圆形
		int method;//拟合方法
		int iters;//迭代次数
		int samples;//抽样点个数
		double reps;//内点距离

		ParaFit() : mode(0), method(7), iters(20), samples(2), reps(2.5) {}
		ParaFit(int _mode) : mode(_mode), method(7), iters(20), samples(2), reps(2.5) {}
	};

	MWWZ_API int measure_pos_d(double xc, double yc, double angle, double len1, double len2, unsigned char* pGray, int w, int h, double sigma, double thr, double*& pRes, int& num);

	//卡尺工具
	MWWZ_API void rr_to_pts(RRect rr, double* pts);
	MWWZ_API void ppw_to_rr(double x1, double y1, double x2, double y2, double roiw, RRect& rr);//p1->p2, roiw:The entire caliper width
	MWWZ_API int gen_measure_line(double x1, double y1, double x2, double y2, double len1, double len2, int dir, int distr, RRect*& pRr, int& num);
	MWWZ_API int gen_measure_circle(double x0, double y0, double radius, double len1, double len2, int dir, int distr, RRect*& pRr, int& num);
	MWWZ_API int do_measure(unsigned char* pGray, int w, int h, double sigma, double thr, const char* sel, RRect* pRr, int num, double*& pRes, int& num2);
	MWWZ_API int do_fit(double* points, int num, int mode, int method, float reps, int iters, int samples, double* pRes);
	MWWZ_API int do_fit2(double* points, int num, ParaFit para, double* pRes);


#ifdef __cplusplus
}
#endif

#ifdef MWWZ_CPLUSPLUS
}
#endif