// test opencv export dll with unity

#include "opencv2\opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "../OpencvDLL/OpencvDll.h"
#include "../FrameProcessOpenCV/ExportOpenCVDll.h"

void testRGBAChannel()
{
 	cv::Mat testImg = cv::Mat::zeros (30,30,CV_8UC4);
	for(int nY = 0; nY <30; nY++){
		for(int nX = 0; nX < 30; nX++){
			testImg.at<cv::Vec4b> (nY,nX) = cv::Vec4b(255,0,0,0);
		}
	}
// 	redImg.at <cv::Vec4b>(0,0) = cv::Vec4b(255,0,0,255);
// 	redImg.at <cv::Vec4b>(1,0) = cv::Vec4b(255,255,0,255);
// 	redImg.at <cv::Vec4b>(2,0) = cv::Vec4b(255,0,255,255);
// 	redImg.at <cv::Vec4b>(3,0) = cv::Vec4b(255,255,255,255);
// 
// 	redImg.at <cv::Vec4b>(0,1) = cv::Vec4b(0,255,0,255);
// 	redImg.at <cv::Vec4b>(1,1) = cv::Vec4b(255,255,0,255);
// 	redImg.at <cv::Vec4b>(2,1) = cv::Vec4b(0,255,255,255);
// 	redImg.at <cv::Vec4b>(3,1) = cv::Vec4b(255,255,255,255);
// 
// 	redImg.at <cv::Vec4b>(0,2) = cv::Vec4b(0,0,255,255);
// 	redImg.at <cv::Vec4b>(1,2) = cv::Vec4b(0,255,255,255);
// 	redImg.at <cv::Vec4b>(2,2) = cv::Vec4b(255,0,255,255);
// 	redImg.at <cv::Vec4b>(3,2) = cv::Vec4b(255,255,255,255);
	imshow ("test channel",testImg);
	imwrite ("testChannel.png",testImg);

	cv::Mat redImg = cv::imread ("red.png");
	cv::Vec4b color = redImg.at<cv::Vec4b> (0,0);
	cv::Mat greenImg = cv::imread ("green.png");
	color = greenImg.at<cv::Vec4b> (0,0);
	cv::Mat blueImg = cv::imread ("blue.png");
	color = blueImg.at<cv::Vec4b> (0,0);
	cv::waitKey ();
}

void testOpencvDll()
{
	cv::Mat img = cv::imread("test.png");
	int width = 640,height = 480, step = 3;
	uchar * imgData = new uchar[width * height * step];
	memset (imgData,0,width * height * step);
	memcpy_s(imgData,width * height * step,img.data,width * height * step);

	// start
	uchar* output = new uchar[width * height * step];
	memset (output,0,width * height * step);
	testUnityPtr (imgData,output,width,height,step);
	cv::Mat outputImg = cv::Mat(height,width,CV_8UC3,output);
	imwrite ("output.jpg",outputImg);
}

void testFrameProcess()
{
	cv::Mat img1 = cv::imread("testClickFingers.jpg");
	cv::Mat img2 = cv::imread("testClickFingers.jpg");
	uchar * img3data = new uchar[img1.rows * img1.cols * 3];
	memset (img3data,0,img1.rows * img1.cols * 3);
	uchar * img4data = new uchar[img1.rows * img1.cols * 3];
	memset (img4data,0,img1.rows * img1.cols * 3);
	int menuActionCnt = 0, clickActionCnt = 0;
	
	//memset(action,FrameProcess::ACTION::NONE,sizeof(action));
	
	ARGlassAction *menuAction = new ARGlassAction[2];
	ARGlassAction *clickAction = new ARGlassAction[2];
	processDoubleFrame(img1.data,img2.data,img3data,img4data,
		img1.cols,img1.rows,3,true,true,
		menuAction,&menuActionCnt,
		clickAction,&clickActionCnt);
// 	circle (img1,menuAction[0].pos,5,cv::Scalar(0,200,0),2);
// 	circle (img1,clickAction[0].pos,5,cv::Scalar(0,200,0),2);
// 	circle (img1,cv::Point(5,5),5,cv::Scalar(0,200,0),2);
// 	imwrite ("MenuClickCircle.jpg",img1);
	imwrite ("outputright.jpg",img2);
}

int main(int argc, char ** argv)
{
	testFrameProcess ();

	return 0;
}