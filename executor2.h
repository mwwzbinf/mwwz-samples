#ifndef EXECUTOR2_H
#define EXECUTOR2_H

#pragma comment(lib, "mwwz.lib")
#ifdef _DEBUG
#pragma comment(lib, "opencv_world450d.lib")
#else
#pragma comment(lib, "opencv_world450.lib")
#endif
#include<vector>
using namespace std;
#include"mwwz.h"
#include<opencv2/opencv.hpp>
using namespace cv;
using namespace mwwz;
using namespace chrono;

class Executor2
{
public:
    Executor2(const std::string name);
    ~Executor2();
    void process();


private:
    int ShowMatches(Mat& Insp, int model_id, int x0, int y0, Match* pMatches, int count, int subpixel = 0);

    int m_model_id=-1;
private:
    String m_name;

    double m_angle_start = -30;
    double m_angle_extent = 60;
    double m_angle_step = 0;
    double m_scale_step = 0;

    int m_contrast_low = 10;
    int m_contrast_high = 15;
    int m_min_cont_len = 30;
    int m_min_contrast = 10;
};
#endif // EXECUTOR2_H
