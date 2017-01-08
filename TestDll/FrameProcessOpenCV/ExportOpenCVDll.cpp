//宏定义  
#define  EXPORTBUILD_OPENCV

#include "ExportOpenCVDll.h"

extern "C" void _DLLExport processDoubleFrame (uchar * inputOutputLeft,uchar * inputOutputRight, 
											   uchar * inputOutputClickLeft,uchar * inputOutputClickRight,
											   int width, int height, int step, bool isPostCalib,bool isStereoRectify, 
											   ARGlassAction * menuAction, int* menuActionCnt,
											   ARGlassAction * clickAction, int * clickActionCnt)
{
	// 为了避免每次都要创建一个opencv mat来进行处理 输入方每次都将output的内容用input初始化再传进来给予修改
	if(!inputOutputLeft || !inputOutputRight)
		return;  

	// 2014-3-25 16:38:16 多出来的一步 先进行变换 匹配两个相机 每帧都要做 感觉好浪费时间= =
	FrameProcess::getInstance ()->setDetail (height,width,step,isPostCalib,isStereoRectify);
	FrameProcess::getInstance ()->setRawImg (inputOutputLeft,inputOutputRight);		// so many cv::zeros maybe effect the performance

	// 2014-8-25 22:21:20 用ocam提供的代码作参考进行undistort 其他的都先不做
	FrameProcess::getInstance ()->undistortion();	

	// assign back
	//imwrite ("inputLeftAfterStereo2.jpg",FrameProcess::getInstance ()->getProcessImgLeft());
	memcpy_s (inputOutputLeft,height * width * step,FrameProcess::getInstance ()->m_cvUndist8UC3_0.data,height * width * step);
	memcpy_s (inputOutputRight,height * width * step,FrameProcess::getInstance ()->m_cvUndist8UC3_1.data,height * width * step);

	return;
	// get视差 以后用的话 对肤色也有影响 得在之前做
	// 不得不说 效果太不明朗了 这个手的判断还是挺差的
	/*FrameProcess::getInstance ()->processDisparity ();*/

	// 得到的数据是argb 但opencv存储的格式是bgr 所以本来蓝色的东西0 0 255 255直接存储就变成了
	// 2014-4-3 14:35:44 我觉得顺序有问题 应该先匹配 然后肤色 然后根据所需视差一下
	FrameProcess::getInstance ()->stereoMatch ();

	// test 立体校正后的两张输入图
	//imshow ("input left after stereo",FrameProcess::getInstance ()->getProcessImgLeft());
	//imwrite ("inputLeftAfterStereo.jpg",FrameProcess::getInstance ()->getProcessImgLeft());
	//imshow ("input right after stereo",FrameProcess::getInstance ()->getProcessImgRight());

	// 指环提取 m_circleMaskLeft/Right 为色环区域
	FrameProcess::getInstance ()->processFingerCircle();
	// 肤色提取 将色环区域也标记为肤色m_cvSkinImgRight/Left
	FrameProcess::getInstance ()->processSkinImg (IMAGE_BOTH);
	// 对肤色/原图进行视差提取
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
