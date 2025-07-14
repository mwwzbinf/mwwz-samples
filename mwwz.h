//#pragma once
#ifndef __MWWZ_H__
#define __MWWZ_H__
#include"apiExport.h"

#ifdef MWWZ_CPLUSPLUS
namespace mwwz
{
#endif

#ifdef __cplusplus           
	extern "C" {
#endif

	struct /*alignas(32)*/ Match
	{
		Match()
		{
		}

		Match(int x, int y, float score);
		Match(int x, int y, float score, float angle);
		Match(int x, int y, float score, float angle, float scale);
		Match(int x, int y, float score, float angle, float scale, int label);
		Match(int x, int y, float score, float angle, float scale, int label, char polarity);
		Match(int x, int y, float score, float angle, float scale, float scale2, int label, char polarity);

		/// Sort matches with high similarity to the front
		bool operator<(const Match& rhs) const
		{
			return score > rhs.score;
		}

		bool operator==(const Match& rhs) const
		{
			return x == rhs.x && y == rhs.y && score == rhs.score;
		}

		int x;
		int y;
		int label;
		float angle;//degree
		float scale;//row
		float scale2;//col
		float score;
		char polarity;
		float dx;
		float dy;
	};
	inline Match::Match(int _x, int _y, float _score)
		: x(_x), y(_y), score(_score)
	{
		dx = 0;
		dy = 0;
	}
	inline Match::Match(int _x, int _y, float _score, float _angle)
		: x(_x), y(_y), score(_score), angle(_angle)
	{
		dx = 0;
		dy = 0;
	}
	inline Match::Match(int _x, int _y, float _score, float _angle, float _scale)
		: x(_x), y(_y), score(_score), angle(_angle), scale(_scale)
	{
		dx = 0;
		dy = 0;
	}
	inline Match::Match(int _x, int _y, float _score, float _angle, float _scale, int _label)
		: x(_x), y(_y), score(_score), angle(_angle), scale(_scale), label(_label)
	{
		dx = 0;
		dy = 0;
	}
	inline Match::Match(int _x, int _y, float _score, float _angle, float _scale, int _label,char _polarity)
		: x(_x), y(_y), score(_score), angle(_angle), scale(_scale), label(_label), polarity(_polarity)
	{
		dx = 0;
		dy = 0;
	}

	inline Match::Match(int _x, int _y, float _score, float _angle, float _scale, float _scale2, int _label, char _polarity)
		: x(_x), y(_y), score(_score), angle(_angle), scale(_scale), scale2(_scale2), label(_label), polarity(_polarity)
	{
		dx = 0;
		dy = 0;
	}

	struct /*alignas(32)*/ Feature
	{
		float x;
		float y;
		float nx;
		float ny;
		Feature() : x(0), y(0), nx(0), ny(0) {}
		Feature(float x, float y, float nx, float ny);
	};
	inline Feature::Feature(float _x, float _y, float _nx, float _ny) : x(_x), y(_y), nx(_nx), ny(_ny) {}

	struct FeatureI
	{
		int x;
		int y;
		float nx;
		float ny;
		int label;
		FeatureI() : x(0), y(0), nx(0), ny(0), label(-1) {}
		FeatureI(int x, int y, float nx, float ny, int label = -1);
	};
	inline FeatureI::FeatureI(int _x, int _y, float _nx, float _ny, int _label) : x(_x), y(_y), nx(_nx), ny(_ny), label(_label) {}

	struct MWWZ_API MatchRes
	{
		int numFound;
		Match* pMatches;
		MatchRes();
		~MatchRes();
	};

	struct MWWZ_API MatchDetails
	{
		int tl_x;
		int tl_y;
		float cx;
		float cy;
		int num_features;
		Feature* features;
		float* scores;
		Match match;
		MatchDetails();
		~MatchDetails();
	};

	struct MWWZ_API Contour
	{
		int num_features;
		double diameter;
		Feature* features;
		Contour();
		~Contour();
	};

	struct MWWZ_API TemplateI
	{
		int tl_x;
		int tl_y;
		int br_x;
		int br_y;
		int width;
		int height;
		int num_features;
		int mem_id;
		FeatureI* features;

		TemplateI();
		~TemplateI();
	};

	struct MWWZ_API TemplateF
	{
		int tl_x;
		int tl_y;
		int br_x;
		int br_y;
		int width;
		int height;
		int num_features;
		int mem_id;
		Feature* features;

		TemplateF();
		~TemplateF();

	};

	// 此类是从 dll 导出的
	class MWWZ_API Cmwwz {
	public:
		Cmwwz(void);
		// TODO: 在此处添加方法。

	};

	extern MWWZ_API int nmwwz;

	MWWZ_API int fnmwwz(void);
	MWWZ_API int init_mwwz();
	MWWZ_API int init_mwwz_lic(const char* str_lic);
	MWWZ_API void set_system(const char* paramName, int paramValue);
	MWWZ_API int edges_sub_pix_gray(unsigned char* pGray, int w, int h, int ksize, float lowThresh, float highThresh, int min_cont_len, int gap, std::vector<Feature>& features, unsigned char* pMask = NULL);
	MWWZ_API int find_sub_pix_contours(unsigned char* pGray, int w, int h, int ksize, int bw, float lowThresh, float highThresh, int min_cont_len, int gap, std::vector<std::vector<Feature>>& contours, unsigned char* pMask = NULL);
	MWWZ_API int edges_sub_pix_gray_d(unsigned char* pGray, int w, int h, float sigma, float lowThresh, float highThresh, float min_dia, std::vector<Feature>& features, unsigned char* pMask = NULL);
	MWWZ_API int find_sub_pix_contours_d(unsigned char* pGray, int w, int h, float sigma, float lowThresh, float highThresh, float min_dia, std::vector<std::vector<Feature>>& contours, unsigned char* pMask = NULL);
	MWWZ_API int find_sub_pix_contours_c(unsigned char* pGray, int w, int h, float sigma, float lowThresh, float highThresh, Contour*& pCont, int& num_cont, unsigned char* pMask = NULL);
	MWWZ_API bool get_subpix_pos(unsigned char* pGray, int w, int h, int x1, int y1, float thr, Feature& f);

	MWWZ_API int clear_mems(int mem_id);
	MWWZ_API int clear_shape_model(int model_id);
	MWWZ_API int write_shape_model(int model_id, const char* file_name);
	MWWZ_API int read_shape_model(const char* file_name, int& model_id);

	MWWZ_API int write_shape_model_b(int model_id, char*& pModel, int& len);
	MWWZ_API int read_shape_model_b(char* pModel, int len, int& model_id);
	MWWZ_API int get_rot_img(unsigned char* pGray, int w, int h, int method, double xc, double yc, double angle, unsigned char* pDst, int w2, int h2);
	//MWWZ_API void auto_shape_model_params(unsigned char* pGray, int w, int h, double ratio, double* T);

	MWWZ_API int create_shape_model(unsigned char* pGray, int w, int h, int num_levels, float angle_start, float angle_extent, float angle_step, int use_polarity, int* contrast, unsigned char* pMask, int& model_id);
	MWWZ_API int create_shape_model_rr(unsigned char* pGray, int w, int h, double xc, double yc, double angle, double len1, double len2, int num_levels, float angle_start, float angle_extent, float angle_step, int use_polarity, int* contrast, unsigned char* pMask, int& model_id);
	MWWZ_API int find_shape_model(unsigned char* pGray, int w, int h, int model_id, float angle_start, float angle_extent, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, Match*& pMatches, int& nFound);
	MWWZ_API int find_scaled_shape_model(unsigned char* pGray, int w, int h, int model_id, float angle_start, float angle_extent, float scale_min, float scale_max, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, Match*& pMatches, int& nFound);
	MWWZ_API int find_aniso_shape_model(unsigned char* pGray, int w, int h, int model_id, float angle_start, float angle_extent, float scale_rmin, float scale_rmax, float scale_cmin, float scale_cmax, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, Match*& pMatches, int& nFound);
	MWWZ_API int find_shape_models(unsigned char* pGray, int w, int h, int* model_id, int model_num, float angle_start, float angle_extent, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, Match*& pMatches, int& nFound);
	
	MWWZ_API int find_shape_model_2(unsigned char* pGray, int w, int h, int model_id, float angle_start, float angle_extent, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, unsigned char* pMask, Match*& pMatches, int& nFound);
	MWWZ_API int find_scaled_shape_model_2(unsigned char* pGray, int w, int h, int model_id, float angle_start, float angle_extent, float scale_min, float scale_max, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, unsigned char* pMask, Match*& pMatches, int& nFound);
	MWWZ_API int find_aniso_shape_model_2(unsigned char* pGray, int w, int h, int model_id, float angle_start, float angle_extent, float scale_rmin, float scale_rmax, float scale_cmin, float scale_cmax, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, unsigned char* pMask, Match*& pMatches, int& nFound);
	MWWZ_API int find_shape_models_2(unsigned char* pGray, int w, int h, int* model_id, int model_num, float angle_start, float angle_extent, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, unsigned char* pMask, Match*& pMatches, int& nFound);

	MWWZ_API int find_shape_model_xl(unsigned char* pGray, int w, int h, int* div, int model_id, float angle_start, float angle_extent, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness,float dsame, Match*& pMatches, int& nFound);
	MWWZ_API int find_shape_models_xl(unsigned char* pGray, int w, int h, int* div, int* model_id, int model_num, float angle_start, float angle_extent, float minScore, int numMatches, float maxOverLap, int subpixel, int* numLevels, float greediness, float dsame, Match*& pMatches, int& nFound);
	MWWZ_API int get_match_details(unsigned char* pGray, int w, int h, float minMag, Match* pMatches, int nFound, MatchDetails*& pDetails);
	MWWZ_API int get_match_details2(unsigned char* pGray, int w, int h, float minMag, Match* pMatches, int nFound, unsigned char* pMask, MatchDetails*& pDetails);

	MWWZ_API TemplateI get_model_transform(int model_id, float angle, float scale, int gapSample = 0);
	MWWZ_API TemplateF get_model_transform2(int model_id, float angle, float scale, int gapSample = 0);
	MWWZ_API TemplateI get_model_transform_s(int model_id, float angle, float scale, float scale2, int gapSample = 0);
	MWWZ_API TemplateF get_model_transform2_s(int model_id, float angle, float scale, float scale2, int gapSample = 0);
	
	MWWZ_API int get_model_contour(int model_id, float angle, float scale, TemplateI*& templ);
	MWWZ_API int get_model_contour2(int model_id, float angle, float scale, TemplateF*& templ);
	MWWZ_API int get_model_contour3(int model_id, int level, float angle, float scale, TemplateF*& templ);
	MWWZ_API int get_model_contour_s(int model_id, float angle, float scale, float scale2, TemplateI*& templ);
	MWWZ_API int get_model_contour2_s(int model_id, float angle, float scale, float scale2, TemplateF*& templ);
	MWWZ_API int get_model_contour3_s(int model_id, int level, float angle, float scale, float scale2, TemplateF*& templ);

	MWWZ_API void set_shape_model_param(int model_id, const char* paramName, int paramValue);
	MWWZ_API void get_shape_model_param(int model_id, const char* paramName, int& paramValue);
	MWWZ_API void set_shape_model_origin(int model_id, float x, float y);
	MWWZ_API void set_match_result_offset(Match& match, float dx, float dy, float& new_x, float& new_y);
	MWWZ_API int copy_shape_model(int model_id);
	MWWZ_API int remove_model_feature(int model_id, int feature_id, float radius);
	MWWZ_API int invert_model_feature(int model_id, int quad);

	MWWZ_API int create_shape_model_xld(float len1, float len2, int num_levels, float angle_start, float angle_extent, float angle_step, int use_polarity, int* contrast, int& model_id);
	MWWZ_API int create_shape_model_xld2(int type, int dir, float len1, float len2, int num_levels, float angle_start, float angle_extent, float angle_step, int use_polarity, int* contrast, int& model_id);
	MWWZ_API int create_shape_model_xld3(float* px, float* py, float* nx, float* ny, int pts, int num_levels, float angle_start, float angle_extent, float angle_step, int use_polarity, int* contrast, int& model_id);
	MWWZ_API int create_shape_model_pol(float* px, float* py, unsigned char* pl, int pts, int num_levels, float angle_start, float angle_extent, float angle_step, int use_polarity, int* contrast, int& model_id);

	MWWZ_API int open_console();
	MWWZ_API int close_console();
	MWWZ_API int clear_console();
	MWWZ_API int write_line(const char* info);

#ifdef __cplusplus
}
#endif
#ifdef MWWZ_CPLUSPLUS
}
#endif
#endif // !__MWWZ_H__