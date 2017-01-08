// test opencv export dll with unity

#include "opencv2\opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "../OpencvDLL/OpencvDll.h"

int main(int argc, char ** argv)
{
	cv::Mat img = cv::imread("test.jpg");
	uchar* output = new uchar[640 * 480 * 4];
	memset (output,0,640*480*4);
	testUnityPtr (img.data,output,640,480,640*4);

	return 0;
}