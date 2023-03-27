#include<my_new/event.h>

void test_cv(){
    cv::Mat img = cv::imread("/home/lanpokn/Documents/2022/pbrt/pbrt-v4/pbrt-v4/images/teaser-transparent-machines.png");
	cv::imshow("test", img);
	cv::waitKey(0);
	return;
}