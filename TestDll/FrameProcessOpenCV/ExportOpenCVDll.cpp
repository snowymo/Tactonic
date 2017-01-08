//�궨��  
#define  EXPORTBUILD_OPENCV

#include "ExportOpenCVDll.h"

extern "C" void _DLLExport processDoubleFrame (uchar * inputOutputLeft,uchar * inputOutputRight, 
											   uchar * inputOutputClickLeft,uchar * inputOutputClickRight,
											   int width, int height, int step, bool isPostCalib,bool isStereoRectify, 
											   ARGlassAction * menuAction, int* menuActionCnt,
											   ARGlassAction * clickAction, int * clickActionCnt)
{
	// Ϊ�˱���ÿ�ζ�Ҫ����һ��opencv mat�����д��� ���뷽ÿ�ζ���output��������input��ʼ���ٴ����������޸�
	if(!inputOutputLeft || !inputOutputRight)
		return;  

	// 2014-3-25 16:38:16 �������һ�� �Ƚ��б任 ƥ��������� ÿ֡��Ҫ�� �о����˷�ʱ��= =
	FrameProcess::getInstance ()->setDetail (height,width,step,isPostCalib,isStereoRectify);
	FrameProcess::getInstance ()->setRawImg (inputOutputLeft,inputOutputRight);		// so many cv::zeros maybe effect the performance

	// 2014-8-25 22:21:20 ��ocam�ṩ�Ĵ������ο�����undistort �����Ķ��Ȳ���
	FrameProcess::getInstance ()->undistortion();	

	// assign back
	//imwrite ("inputLeftAfterStereo2.jpg",FrameProcess::getInstance ()->getProcessImgLeft());
	memcpy_s (inputOutputLeft,height * width * step,FrameProcess::getInstance ()->m_cvUndist8UC3_0.data,height * width * step);
	memcpy_s (inputOutputRight,height * width * step,FrameProcess::getInstance ()->m_cvUndist8UC3_1.data,height * width * step);

	return;
	// get�Ӳ� �Ժ��õĻ� �Է�ɫҲ��Ӱ�� ����֮ǰ��
	// ���ò�˵ Ч��̫�������� ����ֵ��жϻ���ͦ���
	/*FrameProcess::getInstance ()->processDisparity ();*/

	// �õ���������argb ��opencv�洢�ĸ�ʽ��bgr ���Ա�����ɫ�Ķ���0 0 255 255ֱ�Ӵ洢�ͱ����
	// 2014-4-3 14:35:44 �Ҿ���˳�������� Ӧ����ƥ�� Ȼ���ɫ Ȼ����������Ӳ�һ��
	FrameProcess::getInstance ()->stereoMatch ();

	// test ����У�������������ͼ
	//imshow ("input left after stereo",FrameProcess::getInstance ()->getProcessImgLeft());
	//imwrite ("inputLeftAfterStereo.jpg",FrameProcess::getInstance ()->getProcessImgLeft());
	//imshow ("input right after stereo",FrameProcess::getInstance ()->getProcessImgRight());

	// ָ����ȡ m_circleMaskLeft/Right Ϊɫ������
	FrameProcess::getInstance ()->processFingerCircle();
	// ��ɫ��ȡ ��ɫ������Ҳ���Ϊ��ɫm_cvSkinImgRight/Left
	FrameProcess::getInstance ()->processSkinImg (IMAGE_BOTH);
	// �Է�ɫ/ԭͼ�����Ӳ���ȡ
#ifdef _DEBUG
	FrameProcess::getInstance ()->processDisparity ();
#endif // _DEBUG
	FrameProcess::getInstance ()->getAction (IMAGE_BOTH,menuAction,menuActionCnt,clickAction,clickActionCnt);

	// channel test:(r,g,b,a)
	// 	cv::rectangle (imgLeft,cv::Rect(30,30,100,100),cv::Scalar(255,0,0,0),5);
	// 	cv::rectangle (imgLeft,cv::Rect(130,30,100,100),cv::Scalar(0,255,0,0),5);
	// 	cv::rectangle (imgLeft,cv::Rect(230,30,100,100),cv::Scalar(0,0,255,0),5);
	// 	cv::rectangle (imgLeft,cv::Rect(330,30,100,100),cv::Scalar(0,0,0,255),5);
	// assign back
	//imwrite ("inputLeftAfterStereo2.jpg",FrameProcess::getInstance ()->getProcessImgLeft());
	memcpy_s (inputOutputLeft,height * width * step,FrameProcess::getInstance ()->getProcessImgLeft().data,height * width * step);
	memcpy_s (inputOutputRight,height * width * step,FrameProcess::getInstance ()->getProcessImgRight().data,height * width * step);
	memcpy_s (inputOutputClickLeft,height * width * step,FrameProcess::getInstance ()->getProcessClickLeft().data,height * width * step);
	memcpy_s (inputOutputClickRight,height * width * step,FrameProcess::getInstance ()->getProcessClickRight().data,height * width * step);
}

extern "C" int _DLLExport adjustYUVcolor (FrameProcess::YUV yuvPara, int value)
{
	return FrameProcess::getInstance ()->setYuv (yuvPara,value);
}

extern "C" int _DLLExport setBMPara (double uniquenessRatio)
{
	FrameProcess::getInstance ()->addUniquenessRatio (uniquenessRatio);
	return FrameProcess::getInstance ()->getUniquenessRatio ();
}

extern "C" void _DLLExport sethsv (int hValueBot,int hValueTop,int sValueBot, int sValueTop,int vValueBot, int vValueTop)
{
	FrameProcess::getInstance ()->tape_hsv_min[0] = hValueBot;
	FrameProcess::getInstance ()->tape_hsv_min[1] = sValueBot;
	FrameProcess::getInstance ()->tape_hsv_min[2] = vValueBot;
	FrameProcess::getInstance ()->tape_hsv_max[0] = hValueTop;
	FrameProcess::getInstance ()->tape_hsv_min[1] = sValueTop;
	FrameProcess::getInstance ()->tape_hsv_min[2] = vValueTop;
}

extern "C" void _DLLExport setElementSize (int eSize)
{
	FrameProcess::getInstance ()->setElement_size (eSize);
}

extern "C" void _DLLExport initOCAM ()
{
	FrameProcess::getInstance ()->getOcamModel ();
}

// extern "C" void _DLLExport stereoRectify (uchar * inputOutputLeft,uchar * inputOutputRight,int width, int height, int step)
// {
// 	FrameProcess::getInstance ()->stereoMatch ();
// 	// assign back
// 	memcpy_s (inputOutputLeft,height * width * step,FrameProcess::getInstance ()->getProcessImgLeft().data,height * width * step);
// 	memcpy_s (inputOutputRight,height * width * step,FrameProcess::getInstance ()->getProcessImgRight().data,height * width * step);
// }
