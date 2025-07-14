#include "executor2.h"

Executor2::Executor2(const std::string name)
{
    m_name = name;

    init_mwwz();
    //读取模板
    string modelPath = "./data/my_model.shm";

    read_shape_model(modelPath.c_str(), m_model_id);
    std::cout<<"@"<<name<<"\tm_model_id:"<<m_model_id<<std::endl;
    if (m_model_id >= 0) {
        std::cout <<m_name <<"load shape model succeed" << endl ;
    } else {
        std::cout <<m_name<< "load shape model failed" << endl;
    }

    set_system("border_shape_models", 1);
    set_shape_model_param(m_model_id, "metric", 1);//不忽略极性
}

Executor2::~Executor2()
{
    clear_shape_model(m_model_id);//在析构时进行这个清理
}
void Executor2::process(){

    Mat src = imread(m_name+"png", IMREAD_GRAYSCALE);

    //寻找模板
    int subpixel = 1;
    double scale_min = 0.99;
    double scale_max = 1.01;
    double minScore = 0.70;
    double maxOverLap = 0.10;
    double greedness = 0.75;
    int numMatches = 0;

    if (scale_max < scale_min) {
        std::cout <<m_name<< "scale min < max" << endl;
        return ;
    }

    int numLevels[2] = { 0 };
    int mem_id = 0;
    int nFound = 0;
    Match* pMatches = NULL;

    mem_id = find_scaled_shape_model(src.ptr<uchar>(0), src.cols, src.rows,
                                     m_model_id, m_angle_start, m_angle_extent, scale_min,
                                     scale_max, minScore, numMatches, maxOverLap, subpixel,
                                     numLevels, greedness, pMatches, nFound);

    /*std::cout<<"#"<<m_name<<"\tmodel_id:"<<m_model_id<<"\tmem_id:"<<mem_id<<"\tpMatches:"<<pMatches
            <<"\t"<<pMatches[0].x
            <<"\t"<<pMatches[0].y
            <<"\t"<<pMatches[0].score
           <<"\t"<<pMatches[0].scale
           <<std::endl;*/



    if (mem_id < 0) {
        std::cout <<m_name<< "find shape model failed" << endl;
        return ;
    } else {
        Mat srcRgb;
        cvtColor(src, srcRgb, COLOR_GRAY2RGB);
        ShowMatches(srcRgb, m_model_id, 0, 0, pMatches, nFound, subpixel);

        /*int pos = m_name.find_last_of('/');
        string s_dir = m_name.substr(0,pos+1);
		string s_name = m_name.substr(pos + 1);
        imwrite(s_dir +"res_" + s_name + "png", srcRgb);*/
        clear_mems(mem_id);

  //      int ret = 0;
  //      ret = clear_mems(mem_id);//在找到结果后进行这个清理
		//printf("----%4d----%3d\n", mem_id, ret);
    }

    
}


int Executor2::ShowMatches(Mat& Insp, int model_id, int x0, int y0, Match* pMatches, int count, int subpixel)
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
            TemplateI templ = get_model_transform_s(model_id, angle, scale, scale2, 1);

            int tl_x = templ.tl_x;
            int tl_y = templ.tl_y;

            FeatureI* features = templ.features;
            float cy = pMatches[i].y + pMatches[i].dy;
            float cx = pMatches[i].x + pMatches[i].dx;

            //std::cout<<m_name<<"\tret: x = "<<cx<<", y = "<<cy<<", score = "<<pMatches[i].score<<endl;

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
                    printf("i = %3d, x = %8.3f, y = %8.3f, angle = %8.3f, scale = %8.3f, scale2 = %8.3f, score = %5.3f\n", model_id/*i + 1*/, cx, cy, pMatches[i].angle, pMatches[i].scale, pMatches[i].scale2, pMatches[i].score);
                else
                    printf("i = %3d, x = %4d, y = %4d, angle = %8.3f, scale = %8.3f, scale2 = %8.3f, score = %5.3f\n", model_id/*i + 1*/, pMatches[i].x, pMatches[i].y, pMatches[i].angle, pMatches[i].scale, pMatches[i].scale2, pMatches[i].score);
            }
			else
			{
                //printf("************************************** model_id = %2d\n", model_id);
				printf("************************************** model_id = %2d, x = %8.3f, y = %8.3f\n", model_id, cy, cx);
			}
            clear_mems(templ.mem_id);
        }
    }
    return 0;
}

