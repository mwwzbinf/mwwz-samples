#pragma once
#include"apiExport.h"

#define max_run_length 360000
#define rle_represented_with_pointer

#ifdef MWWZ_CPLUSPLUS
namespace mwwz
{
#endif
#ifdef __cplusplus           
	extern "C" {
#endif

		struct chord
		{
			/*int lb;*/
			int cb, ce, r, lb;
			chord(int cbIn, int ceIn, int rIn) : cb(cbIn), ce(ceIn), r(rIn) { lb = 0; }
			chord() : cb(0), ce(0), r(0) { lb = 0; }
			bool operator < (const chord& other) const {
				if (r < other.r || (r == other.r && cb < other.cb)
					|| (r == other.r && cb == other.cb && ce < other.ce)) return true; else return false;
			}
		};

		class MWWZ_API region
		{
		public:
			region();
			region(int rle_num);
			~region();
			region(const region& Region);
			region& operator=(region const& Region);

			int len;
#if defined(rle_represented_with_pointer)
			int* r;
			int* cb;
			int* ce;
#else
			std::vector<int> r;
			std::vector<int> cb;
			std::vector<int> ce;
#endif
			std::vector<chord> runs;
			std::vector<int> rect;
			std::vector<double> rrect;
			std::vector<double> ellipse;

			long area;
			double circularity;
			int imgWidth;
			int imgHeight;
			long  obj_count;//连通区域个数 -1, 0单个, >0多个
			region** regions;

			void to_3();
			void to_1();
		private:
			unsigned* refCount;//引用计数
		};

		MWWZ_API void gen_region_runs2(region* regRes, int* r, int* cb, int* ce, int n);
		MWWZ_API void gen_region_runs(region* regRes, std::vector<int>& r, std::vector<int>& cb, std::vector<int>& ce);
		MWWZ_API void get_region_runs(const region& reg1, std::vector<int>& r, std::vector<int>& cb, std::vector<int>& ce);
		MWWZ_API void gen_se_circle(int R, std::vector<mwwz::chord>& runs);
		MWWZ_API void gen_se_rect(int w, int h, std::vector<mwwz::chord>& runs);
		MWWZ_API void gen_se_cross(int w, int h, std::vector<mwwz::chord>& runs);
		
		MWWZ_API void smallest_rectangle1_y(const region& reg1, int* row1, int* col1, int* row2, int* col2);
		MWWZ_API void threshold_y(unsigned char* image, int width, int height, int min_gray, int max_gray, region* regRes);
		MWWZ_API void union2_y(const region& reg1, const region& reg2, region* regRes);
		MWWZ_API void difference_y(const region& reg1, const region& reg2, region* regRes);
		MWWZ_API void intersection_y(const region& reg1, const region& reg2, region* regRes);
		MWWZ_API int connection_y(const region& reg1, region* regRes, int offset = 1);
		MWWZ_API void compute_feature_y(region& reg1);
		MWWZ_API void fill_up_y(const region& reg1, region* regRes);
		MWWZ_API void erosion2_y(const region& reg1, const region& se, region* regRes, int cx = -1, int cy = -1);
		MWWZ_API void dilation2_y(const region& reg1, const region& se, region* regRes, int cx = -1, int cy = -1);
		MWWZ_API void opening_y(const region& reg1, const region& se, region* regRes);
		MWWZ_API void closing_y(const region& reg1, const region& se, region* regRes);

		MWWZ_API void hit_or_miss_y(const region& reg1, const region& se, const region& se2, region* regRes, int cx, int cy);
		MWWZ_API void hit_or_miss_y2(const region& reg1, const region& se, region* regRes, int cx, int cy);

		MWWZ_API void top_hat_y(const region& reg1, const region& se, region* regRes);
		MWWZ_API void bottom_hat_y(const region& reg1, const region& se, region* regRes);
		MWWZ_API void boundary_y(const region& reg1, region* regRes, const char* boundaryType);//inner,inner_filled,outer
		MWWZ_API void get_region_contour(const region& reg1, std::vector<int>& vx, std::vector<int>& vy);
		MWWZ_API void gen_region_points(region* regRes, std::vector<int>& vx, std::vector<int>& vy);
		MWWZ_API void test_region_point(const region& reg1, int x, int y, int& isInside);


#ifdef __cplusplus
	}
#endif
#ifdef MWWZ_CPLUSPLUS
}
#endif
