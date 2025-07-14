// demo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <conio.h>
#include <winsock.h>
#include <WinUser.h>
#include <iostream>

#define INCLUDE_MEAS
#define INCLUDE_MORPH
#include"executor2.h"

int ShowFeatures(cv::Mat& Insp, int model_id)
{
	int cw = 5;
	int cx = Insp.cols / 2;
	int cy = Insp.rows / 2;

	float angle = 0;
	float scale = 1;
	TemplateI templ = get_model_transform(model_id, angle, scale, 1);
	FeatureI* features = templ.features;
	for (int j = 0; j < templ.num_features; j++)
	{
		int r0 = cy + features[j].y;
		int c0 = cx + features[j].x;
		if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
		{
			int c = 0;
			float x = features[j].x;
			float y = features[j].y;
			Insp.at<cv::Vec3b>(r0, c0) = cv::Vec3b(0, 255, 0);
		}

	}
	clear_mems(templ.mem_id);	
	return 0;
}

int ShowMatches(Mat& Insp, int model_id, int x0, int y0, Match* pMatches, int count, int dx, int dy, int subpixel = 0)
{
	// TODO: 在此处添加实现代码.
	if (count > 0)
	{
		int p = 0;
		int cw = 5;
		for (int i = 0; i < count; i++)
		{
			float angle = pMatches[i].angle;
			float scale = pMatches[i].scale;
			float scale2 = pMatches[i].scale2;
			//TemplateI templ = get_model_transform(model_id < 0 ? pMatches[i].label : model_id, angle, scale, 1);
			TemplateI templ = get_model_transform_s(model_id < 0 ? pMatches[i].label : model_id, angle, scale, scale2, 1);
			int tl_x = templ.tl_x;
			int tl_y = templ.tl_y;
			FeatureI* features = templ.features;
			float cy = pMatches[i].y + pMatches[i].dy;
			float cx = pMatches[i].x + pMatches[i].dx;

			for (int j = 0; j < templ.num_features; j++)
			{
				int r0 = round(cy + features[j].y + y0);
				int c0 = round(cx + features[j].x + x0);
				if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
					Insp.at<Vec3b>(r0, c0) = Vec3b(0, 255, 0);
			}
			int r0 = round(cy + y0);
			int c0 = round(cx + x0);
			if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
			{
				line(Insp, Point(c0 - cw, r0), Point(c0 + cw, r0), Scalar(0, 0, 255), 1);
				line(Insp, Point(c0, r0 - cw), Point(c0, r0 + cw), Scalar(0, 0, 255), 1);
				//Insp.at<Vec3b>(r0, c0) = Vec3b(0, 255, 0);

				string str_score = format("%.3f", pMatches[i].score);
				cv::putText(Insp, str_score,
					Point(pMatches[i].x + x0 + tl_x + 10, pMatches[i].y + y0 + tl_y), FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 255));
				
				if (subpixel > 0)
					printf("i = %3d, x = %8.3f, y = %8.3f, angle = %8.3f, scale = %8.3f, scale2 = %8.3f, score = %5.3f\n", i + 1, cx, cy, pMatches[i].angle, pMatches[i].scale, pMatches[i].scale2, pMatches[i].score);
				else
					printf("i = %3d, x = %4d, y = %4d, angle = %8.3f, scale = %8.3f, scale2 = %8.3f, score = %5.3f\n", i + 1, pMatches[i].x, pMatches[i].y, pMatches[i].angle, pMatches[i].scale, pMatches[i].scale2, pMatches[i].score);
			}

			float new_x, new_y;
			if (dx != 0 || dy != 0)
			{
				set_match_result_offset(pMatches[i], dx, dy, new_x, new_y);
				r0 = round(new_y);
				c0 = round(new_x);
				if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
				{
					line(Insp, Point(c0 - cw, r0), Point(c0 + cw, r0), Scalar(255, 0, 255), 1);
					line(Insp, Point(c0, r0 - cw), Point(c0, r0 + cw), Scalar(255, 0, 255), 1);
				}
			}
			clear_mems(templ.mem_id);
		}
	}
	return 0;
}

int ShowMatchDetails(Mat& Insp, uchar mode, int model_id, int x0, int y0, MatchDetails* pDetails, int count, int subpixel)
{
	// TODO: 在此处添加实现代码.
	if (count > 0)
	{
		int p = 0;
		int cw = 5;
		float dx = 0;
		float dy = 0;
		float score_t = cos(CV_PI / 6);
		for (int i = 0; i < count; i++)
		{
			int tl_x = pDetails[i].tl_x;
			int tl_y = pDetails[i].tl_y;
			float cy = pDetails[i].cy;
			float cx = pDetails[i].cx;
			float* scores = pDetails[i].scores;
			Feature* features = pDetails[i].features;
			int num_features = pDetails[i].num_features;
			Match& match = pDetails[i].match;

			//显示轮廓点
			if (mode & 1 || mode == 0)
			{
				for (int j = 0; j < num_features; j++)
				{
					int r0 = round(cy + features[j].y + y0);
					int c0 = round(cx + features[j].x + x0);
					if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
						Insp.at<Vec3b>(r0, c0) = scores[j] >= score_t ? Vec3b(0, 255, 0) : Vec3b(0, 0, 255);
				}
			}

			int r0 = round(cy + y0);
			int c0 = round(cx + x0);
			if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
			{
				//显示十字
				if (mode & 2 || mode == 0)
				{
					//Insp.at<Vec3b>(r0, c0) = Vec3b(0, 255, 0);
					line(Insp, Point(c0 - cw, r0), Point(c0 + cw, r0), Scalar(0, 0, 255), 1);
					line(Insp, Point(c0, r0 - cw), Point(c0, r0 + cw), Scalar(0, 0, 255), 1);
				}

				//显示分数
				if (mode & 4 || mode == 0)
				{
					string str_score = format("%.3f", match.score);
					cv::putText(Insp, str_score,
						Point(match.x + x0 + tl_x + 10, match.y + y0 + tl_y), FONT_HERSHEY_TRIPLEX, 1, cv::Scalar(0, 0, 255));
				}

				if (subpixel > 0)
				{
					//printf("i = %3d, x = %8.3f, y = %8.3f, angle = %8.3f, scale = %8.3f, score = %5.3f\n", i + 1, cx, cy, match.angle, match.scale, match.score);
					printf("i = %3d, x = %8.3f, y = %8.3f, angle = %8.3f, scale = %8.3f, scale2 = %8.3f, score = %5.3f\n", i + 1, cx, cy, match.angle, match.scale, match.scale2, match.score);
				}
				else
				{
					//printf("i = %3d, x = %4d, y = %4d, angle = %8.3f, scale = %8.3f, score = %5.3f\n", i + 1, match.x, match.y, match.angle, match.scale, match.score);
					printf("i = %3d, x = %4d, y = %4d, angle = %8.3f, scale = %8.3f, scale2 = %8.3f, score = %5.3f\n", i + 1, match.x, match.y, match.angle, match.scale, match.scale2, match.score);
				}
			}
			float new_x, new_y;

			if (dx != 0 || dy != 0)
			{
				set_match_result_offset(match, dx, dy, new_x, new_y);
				r0 = round(new_y);
				c0 = round(new_x);
				if (r0 >= 0 && c0 >= 0 && r0 < Insp.rows && c0 < Insp.cols)
				{
					line(Insp, Point(c0 - cw, r0), Point(c0 + cw, r0), Scalar(255, 0, 255), 1);
					line(Insp, Point(c0, r0 - cw), Point(c0, r0 + cw), Scalar(255, 0, 255), 1);
				}
			}
		}
	}
	return 0;
}

const string img_dir = "./imgs/";
//rotate and set border_shape_models
int test()
{
	init_mwwz();
	Mat model = imread(img_dir + "model.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);
	
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);

	
	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}
	
	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "src.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels,greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound,0,0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//scale and set polarity
int test2()
{
	init_mwwz();
	Mat model = imread(img_dir + "m1.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s1.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 0.5;
	double scale_max = 1.5;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}
	set_shape_model_param(model_id, "metric", 0);//忽略极性

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//create model with mask
int test3()
{
	init_mwwz();
	Mat model = imread(img_dir + "m2.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	uchar* pMask = NULL;
	Mat mask = imread(img_dir + "k2.bmp", IMREAD_GRAYSCALE);
	if (!mask.empty()) 
	{
		Mat se = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));
		morphologyEx(mask, mask, MORPH_ERODE, se);// 腐蚀，去粘连部位的干扰

		pMask = mask.ptr<uchar>(0);
		Mat reduced;
		model.copyTo(reduced, mask);
		imshow("mask", mask);
		imshow("reduced", reduced);
	}

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, pMask, model_id);
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "2a.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//create aniso model from circle (radius, 0)
int test4()
{
	init_mwwz();

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	//半径
	float len1 = 50;
	float len2 = 0;

	int model_id = -1;
	int err = create_shape_model_xld(len1, len2, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, model_id);
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb = Mat::zeros(2 * (len2 > 0 ? len2 : len1) + 4, 2 * len1 + 4, CV_8UC3);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s4.jpg", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 2;
	double scale_min = 0.5;
	double scale_max = 1.5;
	double minScore = 0.70;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}
	set_shape_model_param(model_id, "metric", 0);//忽略极性
	set_shape_model_param(model_id, "timeout", 1000);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	mem_id = find_aniso_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//create aniso model from rectangle (len1,len2)
int test5()
{
	init_mwwz();

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	//长的一半，宽的一半
	float len1 = 60;
	float len2 = 42;

	int model_id = -1;
	int err = create_shape_model_xld(len1, len2, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, model_id);
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb = Mat::zeros(2 * (len2 > 0 ? len2 : len1) + 4, 2 * len1 + 4, CV_8UC3);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s5.jpg", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 0.5;
	double scale_max = 2.0;
	double minScore = 0.70;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}
	set_shape_model_param(model_id, "metric", 0);//忽略极性

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	mem_id = find_aniso_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//create aniso model from image
int test6()
{
	init_mwwz();
	Mat model = imread(img_dir + "m6.png", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	uchar* pMask = NULL;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, pMask, model_id);
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s6.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 0.5;
	double scale_max = 2;
	double minScore = 0.80;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	mem_id = find_aniso_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//multithread safe
#define KEYDOWN( vk ) ( 0x8000 & ::GetAsyncKeyState( vk ) ) // 或GetKeyState(vk)
void fun(Executor2* ex)
{
	ex->process();
}
int test7()
{
	Executor2* ex21 = new Executor2("./data/ex1.");
	Executor2* ex22 = new Executor2("./data/ex2.");
	Executor2* ex23 = new Executor2("./data/ex3.");
	Executor2* ex24 = new Executor2("./data/ex4.");
	Executor2* ex25 = new Executor2("./data/ex5.");
	Executor2* ex26 = new Executor2("./data/ex6.");

	ex21->process();

	int i = 0;
	//for (int i = 0; i < 100; i++)
	while (true) // 按ESC退出
	{
		thread t1{ fun,ex21 };
		thread t2{ fun,ex22 };
		thread t3{ fun,ex23 };
		thread t4{ fun,ex24 };
		thread t5{ fun,ex25 };
		thread t6{ fun,ex26 };

		t1.join();
		t2.join();
		t3.join();
		t4.join();
		t5.join();
		t6.join();
		printf("\n");
		i++;

		if (i == 1000)
		{
			i = 0;
			system("CLS");
		}
		if (KEYDOWN(VK_ESCAPE)) // 按ESC退出
			break;
	}

	delete ex21;
	delete ex22;
	delete ex23;
	delete ex24;
	delete ex25;
	delete ex26;

	return 0;
}

//write model to byte[]
//read model from byte[]
int test8()
{
	init_mwwz();
	Mat model = imread(img_dir + "model.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	int len = 0;
	char* pModel = NULL;
	int bSuccess = write_shape_model_b(model_id, pModel, len);
	if (bSuccess > 0)
	{
		std::cout << "save shape model succeed" << endl;
	}
	else
	{
		std::cout << "save shape model failed" << endl;
		return -1;
	}
	clear_shape_model(model_id);
	model_id = -1;

	//读取模板
	read_shape_model_b(pModel, len, model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}
	clear_mems(bSuccess);

	Mat src = imread(img_dir + "src.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.75;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//max_deformation with find_shape_model
int test9()
{
	init_mwwz();
	Mat model = imread(img_dir + "m9.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 6;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s9.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;//0=none 1=interpolation 2=least-squares
	int max_deformation = 8;//4 8 16 32,目前仅支持4的倍数
	subpixel += max_deformation;

	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//set_shape_model_origin
int test10()
{
	init_mwwz();
	Mat model = imread(img_dir + "model.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "src.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		//标准直角坐标系，在参考点的正右方
		float dx = 25;
		float dy = 25;
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, dx, dy, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//only test accuracy
int test11()
{
	init_mwwz();
	Mat model = imread(img_dir + "m11.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = -0.2;
	double angle_extent = 0.4;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 20;
	int contrast_high = 40;
	int min_cont_len = 5;
	int min_contrast = 5;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "src11.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

int test12()
{
	init_mwwz();
	Mat model = imread(img_dir + "m12.jpg", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	//Mat src = imread("./s13.png", IMREAD_GRAYSCALE);
	Mat src = imread(img_dir + "s12.jpg", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 1;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.75;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//create model from polygon
int test13()
{
	init_mwwz();
	Mat model = imread(img_dir + "m13.png", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 5;
	int min_contrast = 3;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int pts = 12;
	float px[12] = { 291.292,  250.996,  238.881,  202.798,  199.111,  200.033,  202.930,  242.436,  252.840,  291.819,  295.374,  294.058 };
	float py[12] = { 170.403,  148.280,  149.992,  173.169,  180.543,  224.527,  230.584,  253.235,  254.288,  229.926,  223.342,  174.749 };
	uchar pl[12] = { 1,  1,  0,  1,  0,  1,  0,  1,  0,  1,  0,  1 };

	int model_id = -1;
	int err = create_shape_model_pol(px, py, pl, pts, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, model_id);
	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);

	for (int j = 0; j < pts; j++)
	{
		if (pl[j] > 0)
			circle(modelRgb, Point2f(px[j], py[j]), 2, Scalar(0, 255, 0), 2);
		else
			circle(modelRgb, Point2f(px[j], py[j]), 2, Scalar(255, 0, 255), 2);
		if (j > 0 && pl[j] != 0)
			arrowedLine(modelRgb, Point2f(px[j-1], py[j-1]), Point2f(px[j], py[j]), Scalar(0, 0, 255), 2, 8, 0, 0.2);
	}
	imshow("features", modelRgb);

	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s13_2.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 2;
	double scale_min = 0.5;
	double scale_max = 1.5;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//find_shape_models:find multi models
int test14()
{
	init_mwwz();
	int model_num = 3;
	vector<Mat> vModel(model_num);
	for (int i = 0; i < model_num; i++)
		vModel[i] = imread(img_dir + cv::format("m_%d.bmp", i + 1), IMREAD_GRAYSCALE);

	/*for (int i = 0; i < model_num; i++)
	{
		if (vModel[i].empty())
		{
			std::cout << "model empty" << endl;
			return -1;
		}
		imshow(cv::format("model_%d", i + 1), vModel[i]);
	}*/

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 15;
	int contrast_high = 22;
	int min_cont_len = 10;
	int min_contrast = 5;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	vector<int> models;
	for (int i = 0; i < model_num; i++)
	{
		int model_id = -1;
		int err = create_shape_model(vModel[i].ptr<uchar>(0), vModel[i].cols, vModel[i].rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

		//create model failed
		if (model_id < 0)
		{
			std::cout << "create shape model failed" << endl;
			return -1;
		}
		models.push_back(model_id);
	}
	
	//显示模板特征点
	vector<Mat> vModelRgb(model_num);
	for (int i = 0; i < model_num; i++)
	{
		cvtColor(vModel[i], vModelRgb[i], COLOR_GRAY2RGB);
		ShowFeatures(vModelRgb[i], models[i]);
		imshow(cv::format("features_%d", i + 1), vModelRgb[i]);
	}

	Mat src = imread(img_dir + "s_123.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 0;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;
	vector<int> vnum_levels(model_num * 2, 0);
	set_system("border_shape_models", 1);
	
	int nFound = 0;
	Match* pMatches = NULL;
	int	mem_id = find_shape_models(src.ptr<uchar>(0), src.cols, src.rows, &models[0], model_num, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, &vnum_levels[0], greedness, pMatches, nFound);
	if (mem_id < 0)
	{
		std::cout << "find shape models failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, -1, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		imshow("result", srcRgb);
	}
	for (int i = 0; i < model_num; i++)
		clear_shape_model(models[i]);
	waitKey();
	return 0;
}

//show match details
int test15()
{
	init_mwwz();
	Mat model = imread(img_dir + "m15.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s15.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 2;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		//ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);

		MatchDetails* pDetails = NULL;
		int mem_id2 = get_match_details(src.ptr<uchar>(0), src.cols, src.rows, 10, pMatches, nFound, pDetails);
		ShowMatchDetails(srcRgb, 0, -1, 0, 0, pDetails, nFound, subpixel);

		clear_mems(mem_id);
		clear_mems(mem_id2);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//remove model feature by index
int test16()
{
	init_mwwz();
	Mat model = imread(img_dir + "m15.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	int feature_id = 872;
	float radius = 8;
	remove_model_feature(model_id, feature_id, radius);

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);


	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	/*
	clear_shape_model(model_id);

	//读取模板
	read_shape_model(modelPath.c_str(), model_id);
	if (model_id >= 0)
	{
		std::cout << "load shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "load shape model failed" << endl;
		return -1;
	}

	//复制模板
	int new_id = copy_shape_model(model_id);
	if (new_id >= 0)
	{
		std::cout << "copy shape model succeed" << endl << endl;
	}
	else
	{
		std::cout << "copy shape model failed" << endl;
		return -1;
	}
	printf("model id = %d, new id = %d\n", model_id, new_id);
	clear_shape_model(model_id);
	model_id = new_id;
	*/

	Mat src = imread(img_dir + "s15.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	imshow("src", src);

	//寻找模板
	int subpixel = 2;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.50;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	set_system("border_shape_models", 1);

	int numLevels[2] = { 0 };
	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	if (abs(scale_min - 1) < DBL_EPSILON && abs(scale_max - 1) < DBL_EPSILON)
	{
		mem_id = find_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	else
	{
		mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows, model_id, angle_start, angle_extent, scale_min, scale_max, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, pMatches, nFound);
	}
	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		//ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);

		MatchDetails* pDetails = NULL;
		int mem_id2 = get_match_details(src.ptr<uchar>(0), src.cols, src.rows, 10, pMatches, nFound, pDetails);
		ShowMatchDetails(srcRgb, 0, -1, 0, 0, pDetails, nFound, subpixel);

		clear_mems(mem_id);
		clear_mems(mem_id2);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//big image match
int test17()
{
	init_mwwz();
	Mat model = imread(img_dir + "8a.bmp", IMREAD_GRAYSCALE);
	if (model.empty())
	{
		std::cout << "model empty" << endl;
		return -1;
	}
	//imshow("model", model);

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 10;
	int contrast_high = 15;
	int min_cont_len = 30;
	int min_contrast = 10;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	int model_id = -1;
	int err = create_shape_model(model.ptr<uchar>(0), model.cols, model.rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

	//create model failed
	if (model_id < 0)
	{
		std::cout << "create shape model failed" << endl;
		return -1;
	}

	//显示模板特征点
	Mat modelRgb;
	cvtColor(model, modelRgb, COLOR_GRAY2RGB);
	ShowFeatures(modelRgb, model_id);
	imshow("features", modelRgb);

	//保存模板
	string modelPath = "my_model.shm";
	int bSuccess = write_shape_model(model_id, modelPath.c_str());
	if (bSuccess == 0)
	{
		std::cout << "save shape model succeed" << endl;
	}

	Mat src = imread(img_dir + "5a.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	//imshow("src", src);

	//寻找模板
	int subpixel = 2;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.70;
	double maxOverLap = 0.20;
	double greedness = 0.75;
	int numMatches = 0;
	int div[4] = { 0 };
	int numLevels[2] = { 0 };

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	mem_id = find_shape_model_xl(src.ptr<uchar>(0), src.cols, src.rows, div, model_id, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, 2.5f, pMatches, nFound);

	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, model_id, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		namedWindow("result", 0);
		imshow("result", srcRgb);
	}
	clear_shape_model(model_id);
	waitKey();
	return 0;
}

//big image match:models
int test18()
{
	init_mwwz();
	int model_num = 3;
	vector<Mat> vModel(model_num);
	for (int i = 0; i < model_num; i++)
		vModel[i] = imread(img_dir + cv::format("m23_%d.png", i + 1), IMREAD_GRAYSCALE);

	/*for (int i = 0; i < model_num; i++)
	{
		if (vModel[i].empty())
		{
			std::cout << "model empty" << endl;
			return -1;
		}
		imshow(cv::format("model_%d", i + 1), vModel[i]);
	}*/

	//创建模板
	double angle_start = 0;
	double angle_extent = 360;
	double angle_step = 0;
	double scale_step = 0;

	int contrast_low = 15;
	int contrast_high = 30;
	int min_cont_len = 8;
	int min_contrast = 5;
	int contrast[4] = { contrast_low,contrast_high,min_cont_len ,min_contrast };

	int num_levels = 0;
	int use_polarity = 1;

	vector<int> models;
	for (int i = 0; i < model_num; i++)
	{
		int model_id = -1;
		int err = create_shape_model(vModel[i].ptr<uchar>(0), vModel[i].cols, vModel[i].rows, num_levels, angle_start, angle_extent, angle_step, use_polarity, contrast, NULL, model_id);

		//create model failed
		if (model_id < 0)
		{
			std::cout << "create shape model failed" << endl;
			return -1;
		}
		models.push_back(model_id);
	}

	//显示模板特征点
	vector<Mat> vModelRgb(model_num);
	for (int i = 0; i < model_num; i++)
	{
		cvtColor(vModel[i], vModelRgb[i], COLOR_GRAY2RGB);
		ShowFeatures(vModelRgb[i], models[i]);
		imshow(cv::format("features_%d", i + 1), vModelRgb[i]);
	}

	Mat src = imread(img_dir + "5a.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "src empty" << endl;
		return -1;
	}
	//imshow("src", src);

	//寻找模板
	int subpixel = 2;
	double scale_min = 1;
	double scale_max = 1;
	double minScore = 0.70;
	double maxOverLap = 0.50;
	double greedness = 0.75;
	int numMatches = 0;
	int div[4] = { 0 };
	int numLevels[2] = { 0 };

	if (scale_max < scale_min)
	{
		std::cout << "scale min < max" << endl;
		return -1;
	}

	int mem_id = 0;
	int nFound = 0;
	Match* pMatches = NULL;
	mem_id = find_shape_models_xl(src.ptr<uchar>(0), src.cols, src.rows, div, &models[0], model_num, angle_start, angle_extent, minScore, numMatches, maxOverLap, subpixel, numLevels, greedness, 2.5f, pMatches, nFound);

	if (mem_id < 0)
	{
		std::cout << "find shape model failed" << endl;
		return -1;
	}
	else
	{
		Mat srcRgb;
		cvtColor(src, srcRgb, COLOR_GRAY2RGB);
		ShowMatches(srcRgb, -1, 0, 0, pMatches, nFound, 0, 0, subpixel);
		clear_mems(mem_id);
		namedWindow("result", 0);
		imshow("result", srcRgb);
	}
	for (int i = 0; i < model_num; i++)
		clear_shape_model(models[i]);
	waitKey();
	return 0;
}

#if defined(INCLUDE_MEAS)
#include"./meas.h"
void draw_rrect(Mat& img, RRect rr)
{
	double pts[12];
	rr_to_pts(rr, pts);
	double y0 = pts[1];
	double x0 = pts[0];
	double y1 = pts[3];
	double x1 = pts[2];
	vector<double> vx(5, 0);
	vector<double> vy(5, 0);
	vy[0] = pts[5];
	vx[0] = pts[4];
	vy[1] = pts[7];
	vx[1] = pts[6];
	vy[2] = pts[9];
	vx[2] = pts[8];
	vy[3] = pts[11];
	vx[3] = pts[10];
	vy[4] = vy[0];
	vx[4] = vx[0];

	arrowedLine(img, Point2f(x0, y0), Point2f(x1, y1), Scalar(0, 0, 255), 1);
	for (int i = 0; i < 4; i++)
		line(img, Point2f(vx[i], vy[i]), Point2f(vx[i + 1], vy[i + 1]), Scalar(0, 255, 0), 1);
}

void test_measure_pos()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	assert(!src.empty());

	size_t rows = src.rows;
	size_t cols = src.cols;
	int chans = src.channels();

	double y0 = 13;
	double x0 = 37;
	double y1 = 13;
	double x1 = 370;
	double roiw = 5;//一半宽度

	RRect rr;
	ppw_to_rr(x0, y0, x1, y1, roiw, rr);

	double dThr = 30;
	double dSigma = 1.5;

	int nResult = 0;
	double* pRes = 0;
	int mem_id = measure_pos_d(rr.x, rr.y, rr.angle, rr.len1, rr.len2, src.data, src.cols, src.rows, dSigma, dThr, pRes, nResult);

	printf("measure_pos:\n");
	for (int i = 0; i < nResult; i += 3)
		printf("i = %3d, x = %9.4f, y = %9.4f, amp = %9.4f\n", i + 1, pRes[i], pRes[i + 1], pRes[i + 2]);

	//将结果画出来
	Mat srcRgb;
	cvtColor(src, srcRgb, COLOR_GRAY2RGB);
	int aw = 4;
	arrowedLine(srcRgb, Point2f(x0, y0), Point2f(x1, y1), Scalar(0, 0, 255), 1);
	//arrowedLine(srcRgb, Point2f(x0, y0), Point2f(x1, y1), Scalar(255, 0, 255), 2, 8, 0, 0.3);
	for (int i = 0; i < nResult; i += 3)
	{
		line(srcRgb, Point2d(pRes[i] - aw, pRes[i + 1] - aw), Point(pRes[i] + aw, pRes[i + 1] + aw), Scalar(0, 255, 0), 1);
		line(srcRgb, Point(pRes[i] - aw, pRes[i + 1] + aw), Point(pRes[i] + aw, pRes[i + 1] - aw), Scalar(0, 255, 0), 1);
	}

	clear_mems(mem_id);
	imshow("measure_pos", srcRgb);
	waitKey();
}

void test_line_detector()
{
	Mat src = imread(img_dir + "Dst7.bmp", IMREAD_GRAYSCALE);
	assert(!src.empty());

	size_t rows = src.rows;
	size_t cols = src.cols;
	int chans = src.channels();

	Mat srcRgb, srcRgb2;
	cvtColor(src, srcRgb, COLOR_GRAY2RGB);
	cvtColor(src, srcRgb2, COLOR_GRAY2RGB);
	imshow("src", srcRgb);

	int aw = 10;
	double x0 = 274;
	double y0 = 354;
	double x1 = 510;
	double y1 = 444;

	arrowedLine(srcRgb, Point2f(x0, y0), Point2f(x1, y1), Scalar(255, 0, 255), 2, 8, 0, 0.3);

	int dir = 1;//顺时针/逆时针 向内/向外
	int distr = 0;//多少个卡尺,0自动计算
	double len1 = 40;
	double len2 = 10;
	RRect* pRr;
	int num = 0;
	int mem_id = gen_measure_line(x0, y0, x1, y1, len1, len2, dir, distr, pRr, num);

	double sigma = 1;//平滑
	double thr = 30;//梯度
	double* points;//结果
	int num2 = 0;
	int mem_id2 = do_measure(src.data, src.cols, src.rows, sigma, thr, "first+", pRr, num, points, num2);

	double ab[4];
	int err_code = do_fit(points, num2, 0, 7, 2, 20, 2, ab);

	printf("num1 = %2d, num2 = %2d\n", num, num2);
	printf("mem_id = %3d, mem_id2 = %3d, err_code = %3d\n", mem_id, mem_id2, err_code);

	for (int i = 0; i < num; i++) draw_rrect(srcRgb, pRr[i]);
	for (int i = 0; i < num2; i++)
	{
		line(srcRgb2, Point2f(points[i] - aw, points[i + num2] - aw), Point2f(points[i] + aw, points[i + num2] + aw), Scalar(255, 0, 255), 1);
		line(srcRgb2, Point2f(points[i] - aw, points[i + num2] + aw), Point2f(points[i] + aw, points[i + num2] - aw), Scalar(255, 0, 255), 1);
	}

	Point2d p1, p2;
	//y = kx+b =>(k,b)
	double k = ab[1] / ab[0];
	double b = ab[3] - k * ab[2];
	p1.x = 0;
	p1.y = k * p1.x + b;
	p2.x = src.cols - 1;
	p2.y = k * p2.x + b;
	cv::line(srcRgb2, p1, p2, Scalar(255, 255, 0), 1);

	clear_mems(mem_id);
	clear_mems(mem_id2);
	imshow("src", srcRgb);
	imshow("result", srcRgb2);
	waitKey();
}

void test_circle_detector()
{
	Mat src = imread(img_dir + "circle.jpg", IMREAD_GRAYSCALE);
	assert(!src.empty());
	Mat srcRgb, srcRgb2;;
	cvtColor(src, srcRgb, COLOR_GRAY2RGB);
	cvtColor(src, srcRgb2, COLOR_GRAY2RGB);
	imshow("src", srcRgb);

	double xc = 656;
	double yc = 530;
	double radius = 273.198;

	int aw = 10;
	line(srcRgb, Point2f(xc - aw, yc), Point2f(xc + aw, yc), Scalar(0, 0, 255), 2);
	line(srcRgb, Point2f(xc, yc - aw), Point2f(xc, yc + aw), Scalar(0, 0, 255), 2);
	circle(srcRgb, Point2f(xc, yc), round(radius), Scalar(0, 255, 0), 2);

	int dir = -1;
	int distr = 0;
	double len1 = 40;
	double len2 = 10;
	RRect* pRr;
	int num = 0;
	int mem_id = gen_measure_circle(xc, yc, radius, len1, len2, dir, distr, pRr, num);

	double sigma = 1;
	double thr = 30;
	double* points;
	int num2 = 0;
	int mem_id2 = do_measure(src.data, src.cols, src.rows, sigma, thr, "first-", pRr, num, points, num2);

	double ab[4];
	int err_code = do_fit(points, num2, 1, 7, 2, 20, 2, ab);

	printf("num1 = %2d, num2 = %2d\n", num, num2);
	printf("mem_id = %3d, mem_id2 = %3d, err_code = %3d\n", mem_id, mem_id2, err_code);

	for (int i = 0; i < num; i++) draw_rrect(srcRgb, pRr[i]);
	for (int i = 0; i < num2; i++)
	{
		line(srcRgb2, Point2f(points[i] - aw, points[i + num2] - aw), Point2f(points[i] + aw, points[i + num2] + aw), Scalar(255, 0, 255), 1);
		line(srcRgb2, Point2f(points[i] - aw, points[i + num2] + aw), Point2f(points[i] + aw, points[i + num2] - aw), Scalar(255, 0, 255), 1);
	}
	cv::circle(srcRgb2, Point2f(ab[0], ab[1]), round(ab[2]), Scalar(255, 255, 0), 1);

	clear_mems(mem_id);
	clear_mems(mem_id2);
	imshow("src", srcRgb);
	imshow("result", srcRgb2);
	waitKey();
}

void test20()
{
	test_measure_pos();

	test_line_detector();

	test_circle_detector();
}
#endif

int  min_gray = 100;
int  max_gray = 255;

#if defined(INCLUDE_MORPH)
#include"./morph.h"
Mat paint_region(const mwwz::region& regIn, int width, int height, Vec3b color = Vec3b(0, 255, 0))
{
	Mat dst;
	int top = 0;
	int left = 0;
	if (width <= 0 || height <= 0)
	{
		int cmin = std::numeric_limits<int>::max();
		int rmin = std::numeric_limits<int>::max();
		int cmax = std::numeric_limits<int>::min();
		int rmax = std::numeric_limits<int>::min();
		if (regIn.len > 0)
		{
			for (int i = 0; i < regIn.len; i++)
			{
				cmin = min(cmin, regIn.cb[i]);
				rmin = min(rmin, regIn.r[i]);
				cmax = max(cmax, regIn.ce[i]);
				rmax = max(rmax, regIn.r[i]);
			}
		}
		else if (regIn.runs.size() > 0)
		{
			const std::vector<mwwz::chord>& runs = regIn.runs;
			for (int i = 0; i < runs.size(); i++)
			{
				const mwwz::chord& run = runs[i];
				cmin = min(cmin, run.cb);
				rmin = min(rmin, run.r);
				cmax = max(cmax, run.ce);
				rmax = max(rmax, run.r);
			}
		}

		height = rmax - rmin + 1;
		width = cmax - cmin + 1;
		top = rmin;
		left = cmin;
	}
	if (width <= 0 || height <= 0) return dst;

	//dst = Mat::zeros(height, width, CV_8UC1);
	dst = Mat::zeros(height, width, CV_8UC3);
	if (regIn.len > 0)
	{
		for (int i = 0; i < regIn.len; i++)
		{
			for (int j = regIn.cb[i]; j <= regIn.ce[i]; j++)
			{
				int i0 = regIn.r[i] - top;
				int j0 = j - left;
				if (j0 >= 0 && j0 < width && i0 >= 0 && i0 < height)
					//dst.at<uchar>(i0, j0) = 255;
					dst.at<Vec3b>(i0, j0) = color;
			}
		}
	}
	else if (regIn.runs.size() > 0)
	{
		const std::vector<mwwz::chord>& runs = regIn.runs;
		for (int i = 0; i < runs.size(); i++)
		{
			const mwwz::chord& run = runs[i];
			for (int j = run.cb; j <= run.ce; j++)
			{
				int i0 = run.r - top;
				int j0 = j - left;
				if (j0 >= 0 && j0 < width && i0 >= 0 && i0 < height)
					//dst.at<uchar>(i0, j0) = 255;
					dst.at<Vec3b>(i0, j0) = color;
			}
		}
	}
	else if (regIn.obj_count > 0)
	{
		for (int n = 0; n < regIn.obj_count; n++)
		{
			const mwwz::region& regTmp = *regIn.regions[n];
			for (int i = 0; i < regTmp.len; i++)
			{
				for (int j = regTmp.cb[i]; j <= regTmp.ce[i]; j++)
				{
					int i0 = regTmp.r[i] - top;
					int j0 = j - left;
					if (j0 >= 0 && j0 < width && i0 >= 0 && i0 < height)
						//dst.at<uchar>(i0, j0) = 255;
						dst.at<Vec3b>(i0, j0) = color;
				}
			}
		}
	}
	return dst;
}

void test_threshold()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	mwwz::compute_feature_y(reg);
	printf("area = %ld, run length = %d\n", reg.area, reg.len);

	Mat img = paint_region(reg, width, height);
	imshow("threshold", img);
	waitKey();
}

void test_union()
{
	Mat src = imread(img_dir + "5a.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}
	Mat src2 = imread(img_dir + "7a.bmp", IMREAD_GRAYSCALE);
	if (src2.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	int width2 = src2.cols;
	int height2 = src2.rows;
	int width3 = max(width, width2);
	int height3 = max(height, height2);

	mwwz::region reg1;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg1);

	mwwz::region reg2;
	mwwz::threshold_y(src2.ptr(0), width2, height2, min_gray, max_gray, &reg2);

	mwwz::region regOut;
	mwwz::union2_y(reg1, reg2, &regOut);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg1, width, height);
	Mat img2 = paint_region(reg2, width2, height2);
	Mat img3 = paint_region(regOut, width3, height3, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("src2", img2);
	imshow("union", img3);
	waitKey();
}

void test_difference()
{
	Mat src = imread(img_dir + "5a.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}
	Mat src2 = imread(img_dir + "7a.bmp", IMREAD_GRAYSCALE);
	if (src2.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	int width2 = src2.cols;
	int height2 = src2.rows;
	int width3 = max(width, width2);
	int height3 = max(height, height2);

	mwwz::region reg1;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg1);

	mwwz::region reg2;
	mwwz::threshold_y(src2.ptr(0), width2, height2, min_gray, max_gray, &reg2);

	mwwz::region regOut;
	mwwz::difference_y(reg1, reg2, &regOut);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg1, width, height);
	Mat img2 = paint_region(reg2, width2, height2);
	Mat img3 = paint_region(regOut, width3, height3, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("src2", img2);
	imshow("difference", img3);
	waitKey();
}

void test_intersection()
{
	Mat src = imread(img_dir + "5a.bmp", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}
	Mat src2 = imread(img_dir + "7a.bmp", IMREAD_GRAYSCALE);
	if (src2.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	int width2 = src2.cols;
	int height2 = src2.rows;
	int width3 = max(width, width2);
	int height3 = max(height, height2);

	mwwz::region reg1;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg1);

	mwwz::region reg2;
	mwwz::threshold_y(src2.ptr(0), width2, height2, min_gray, max_gray, &reg2);

	mwwz::region regOut;
	mwwz::intersection_y(reg1, reg2, &regOut);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg1, width, height);
	Mat img2 = paint_region(reg2, width2, height2);
	Mat img3 = paint_region(regOut, width3, height3, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("src2", img2);
	imshow("intersection", img3);
	waitKey();
}

void test_connection()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	mwwz::region regOut;
	int n = mwwz::connection_y(reg, &regOut);
	printf("connected components = %d\n", n);

	Mat img = Mat::zeros(height, width, CV_8UC3);
	for (int n = 0; n < regOut.obj_count; n++)
	{
		Vec3b color(rand() & 255, rand() & 255, rand() & 255);
		const mwwz::region& regTmp = *regOut.regions[n];
		for (int i = 0; i < regTmp.len; i++)
		{
			for (int j = regTmp.cb[i]; j <= regTmp.ce[i]; j++)
			{
				int i0 = regTmp.r[i];
				int j0 = j;
				if (j0 >= 0 && j0 < width && i0 >= 0 && i0 < height)
					img.at<Vec3b>(i0, j0) = color;
			}
		}
	}
	imshow("connection", img);
	waitKey();
}

void test_erosion()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	//strunct element 1:circle 
	int R = 10;
	std::vector<mwwz::chord> runs;
	mwwz::gen_se_circle(R, runs);
	int len = runs.size();
	mwwz::region regSe(len);
	for (int i = 0; i < len; i++)
	{
		regSe.r[i] = runs[i].r;
		regSe.cb[i] = runs[i].cb;
		regSe.ce[i] = runs[i].ce;
	}
	regSe.len = len;

	//struct element 2
	int n = 18;
	int r[] = { 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23 };
	int cb[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	int ce[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	mwwz::region regSe2;
	mwwz::gen_region_runs2(&regSe2, r, cb, ce, n);

	int ref_y = 1;
	int ref_x = 1;

	mwwz::region regOut;
	mwwz::erosion2_y(reg, regSe, &regOut, ref_x, ref_y);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg, width, height);
	Mat img2 = paint_region(regSe, -1, -1);
	Mat img3 = paint_region(regOut, width, height, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("se", img2);
	imshow("erosion2", img3);
	waitKey();
}

void test_dilation()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	//strunct element 1:circle 
	int R = 10;
	std::vector<mwwz::chord> runs;
	mwwz::gen_se_circle(R, runs);
	int len = runs.size();
	mwwz::region regSe(len);
	for (int i = 0; i < len; i++)
	{
		regSe.r[i] = runs[i].r;
		regSe.cb[i] = runs[i].cb;
		regSe.ce[i] = runs[i].ce;
	}
	regSe.len = len;

	//struct element 2
	int n = 18;
	int r[] = { 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23 };
	int cb[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	int ce[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	mwwz::region regSe2;
	mwwz::gen_region_runs2(&regSe2, r, cb, ce, n);

	int ref_y = 1;
	int ref_x = 1;

	mwwz::region regOut;
	mwwz::dilation2_y(reg, regSe, &regOut, ref_x, ref_y);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg, width, height);
	Mat img2 = paint_region(regSe, -1, -1);
	Mat img3 = paint_region(regOut, width, height, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("se", img2);
	imshow("dilation2", img3);
	waitKey();
}

void test_opening()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	//strunct element 1:circle 
	int R = 10;
	std::vector<mwwz::chord> runs;
	mwwz::gen_se_circle(R, runs);
	int len = runs.size();
	mwwz::region regSe(len);
	for (int i = 0; i < len; i++)
	{
		regSe.r[i] = runs[i].r;
		regSe.cb[i] = runs[i].cb;
		regSe.ce[i] = runs[i].ce;
	}
	regSe.len = len;

	//struct element 2
	int n = 18;
	int r[] = { 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23 };
	int cb[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	int ce[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	mwwz::region regSe2;
	mwwz::gen_region_runs2(&regSe2, r, cb, ce, n);

	mwwz::region regOut;
	mwwz::opening_y(reg, regSe, &regOut);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg, width, height);
	Mat img2 = paint_region(regSe, -1, -1);
	Mat img3 = paint_region(regOut, width, height, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("se", img2);
	imshow("opening", img3);
	waitKey();
}

void test_closing()
{
	Mat src = imread(img_dir + "4a.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	//strunct element 1:circle 
	int R = 10;
	std::vector<mwwz::chord> runs;
	mwwz::gen_se_circle(R, runs);
	int len = runs.size();
	mwwz::region regSe(len);
	for (int i = 0; i < len; i++)
	{
		regSe.r[i] = runs[i].r;
		regSe.cb[i] = runs[i].cb;
		regSe.ce[i] = runs[i].ce;
	}
	regSe.len = len;

	//struct element 2
	int n = 18;
	int r[] = { 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23 };
	int cb[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	int ce[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	mwwz::region regSe2;
	mwwz::gen_region_runs2(&regSe2, r, cb, ce, n);

	mwwz::region regOut;
	mwwz::closing_y(reg, regSe, &regOut);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg, width, height);
	Mat img2 = paint_region(regSe, -1, -1);
	Mat img3 = paint_region(regOut, width, height, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("se", img2);
	imshow("closing", img3);
	waitKey();
}

void test_hit_or_miss()
{
	Mat src = imread(img_dir + "hit-and-miss.png", IMREAD_GRAYSCALE);
	if (src.empty())
	{
		std::cout << "image data is null" << endl;
		return;
	}

	int  width = src.cols;
	int  height = src.rows;
	mwwz::region reg;
	mwwz::threshold_y(src.ptr(0), width, height, min_gray, max_gray, &reg);

	int n2 = 18; //hit 
	int r2[] = { 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23 };
	int cb2[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	int ce2[] = { 282, 280, 284, 279, 286, 279, 286, 278, 287, 278, 287, 279, 286, 279, 286, 280, 284, 282 };
	int n3 = 13;//miss
	int r3[] = { 15, 16, 17, 17, 18, 19, 20, 20, 21, 22, 22, 23, 23 };
	int cb3[] = { 281, 280, 280, 287, 279, 279, 280, 287, 280, 281, 285, 281, 283 };
	int ce3[] = { 283, 285, 285, 287, 286, 286, 285, 287, 285, 283, 285, 281, 283 };
	int ref_y = 14;
	int ref_x = 282;

	mwwz::region regSe(n2);
	mwwz::gen_region_runs2(&regSe, r2, cb2, ce2, n2); regSe.to_3();
	mwwz::region regSe2(n3);
	mwwz::gen_region_runs2(&regSe2, r3, cb3, ce3, n3); regSe2.to_3();

	mwwz::region regOut;
	mwwz::hit_or_miss_y(reg, regSe, regSe2, &regOut, ref_x, ref_y);

	mwwz::compute_feature_y(regOut);
	printf("area = %ld, run length = %d\n", regOut.area, regOut.len);

	Mat img = paint_region(reg, width, height);
	Mat img2 = paint_region(regSe, -1, -1);
	Mat img3 = paint_region(regOut, width, height, Vec3b(0, 0, 255));
	imshow("src", img);
	imshow("se", img2);
	imshow("hit_or_miss", img3);
	moveWindow("hit_or_miss", 100, 100);
	waitKey();
}
#endif

int main()
{
	//(1)shape match
	//test();
	//test3();
	test14();
	//test15();
	//test17();
	//test18();

	//(2)measurement
	//test20();

	//(3)morphology
	//test_threshold();
	//test_union();
	//test_difference();
	//test_intersection();
	//test_connection();
	//test_erosion();
	//test_dilation();
	//test_opening();
	//test_closing();
	//test_hit_or_miss();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
