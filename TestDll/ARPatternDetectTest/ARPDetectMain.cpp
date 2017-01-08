#include "../ARPatternDetect/ExportARPatternDll.h"
#include "opencv2\opencv.hpp"
#include <iostream>

struct ARCSPattern
{
public:
	char * m_patt_name;
	char * m_config_name;
	int m_patt_id;
	int m_model_id;
	int m_visible;
	double m_width;
	double * m_center;
	double m_trans[3][4];
	double m_rotation[4];	
	double m_translation[3];
};

void testARPattern()
{
	cv::Mat img1 = cv::imread("testARToolkitMixed.jpg");
	cv::Mat img2 = cv::imread("leftImg.jpg");
	double gl_para[16];
	float glm_point[32];
	double rotation[4];
	double translation[3];
	cvtColor (img1,img1,CV_RGB2RGBA);
	cvtColor (img2,img2,CV_RGB2RGBA);
	bool isInitial[1] = {false};
	initPattern ();
	//ARPattern arPattern[3];
	ARTCSPattern artPattern[3];

 	bool isDetect = detectPattern (img2.data,img2.data,artPattern);
 	std::cout << "isDetect:" << isDetect << "\tartPattern[2].m_visible:" << artPattern[2].m_visible << "\n";
 	//std::cout << "glm_point:" << glm_point[0] << "," << glm_point[1] << "," << glm_point[2] << "\n";
 	isDetect = detectPattern (img2.data,img2.data,artPattern);
 	std::cout << "isDetect:" << isDetect << "\tartPattern[0].m_visible:" << artPattern[0].m_visible << "\n";
 	//std::cout << "glm_point:" << glm_point[0] << "," << glm_point[1] << "," << glm_point[2] << "\n";
 	
 	
}

int main()
{
	testARPattern ();
	
	return 0;
}