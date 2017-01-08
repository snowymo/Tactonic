//�궨��  
#define  EXPORTBUILD  

#include "OpencvDll.h"
#include <string>
#include "opencv2\core\core.hpp"

extern "C" void _DLLExport testUnityPtr (uchar * imageData, uchar * outputData,int width, int height, int step)
{
	if(NULL==imageData)  
	{  
		return;  
	}
	int type = (step == 3)? CV_8UC3 : CV_8UC4;
	cv::Mat inputImg = cv::Mat(height,width,type,imageData);
	//cvtColor (inputImg,inputImg,CV_RGBA2BGRA);
	//cv::imwrite ("d:\\fromUnityInput.jpg",inputImg);
	//cv::Mat outputImg = cv::Mat(height,width,type,outputData);
	//cvtColor (outputImg,outputImg,CV_RGBA2BGRA);
	//cvtColor (outputImg,outputImg,CV_RGBA)
	//cv::imwrite ("d:\\fromUnityOutput.jpg",outputImg);
	//cv::Mat opencvImage(height,width,CV_8UC4);
	cv::Mat opencvImage = cv::Mat::zeros (height,width,type);
	//memcpy(opencvImage.data,imageData,height * width * step);
	// �õ���������rgba ��opencv�洢�ĸ�ʽ��bgr ���Ա�����ɫ�Ķ���0 0 255 255ֱ�Ӵ洢�ͱ����
	// ������ unity���Դ���bgra32������
	// ����ֻ��ARGB32������setPixels32 ��ú ���opencv��ת�� ���Լ���
	memcpy_s (opencvImage.data,height * width * step,imageData,height * width * step);
	//cvtColor (opencvImage,opencvImage,CV_BGR2RGB);
	//cv::Rect rect(30,30,100,100);
	//cv::rectangle (opencvImage,rect,cv::Scalar(120),5);
	//cv::imwrite ("d:\\afterRect.jpg",opencvImage);
	memcpy_s (outputData,height * width * step,opencvImage.data,height * width * step);
	//memcpy_s (imageData,height * width * step,opencvImage.data,height * width * step);
	//memcpy(outputData,opencvImage.data,height * width * step);
	//imshow("unity capture",opencvImage);  
	//cv::waitKey();
}
