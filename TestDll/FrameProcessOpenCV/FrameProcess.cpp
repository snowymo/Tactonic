#include "FrameProcess.h"
#include "opencv2\imgproc\imgproc.hpp"
#include <string>
#include <fstream>
//#include <glm/glm.hpp>

FrameProcess::FrameProcess(void)
{
	m_yuv1downside = 85;
	m_yuv1upside  = 140;
	m_yuv2downside = 140;
	m_yuv2upside = 298;
	epsilon = 0.015;
	isClosed = true;
	isFound = false;
	edge = 12;
	m_cvSkinImgLeft = NULL;
	m_cvSkinImgRight = NULL;
	m_bInitSkin = false;
	//createCalibrationMatrix ();
	createStereoPara ();
	// step 1 文件输入参数 这个应该放在初始化的地方
	getIntrExtrinsicInput ();
	m_handInfos = new HandInfo[2];
#ifdef _DEBUG
	tape_hsv_min = cvScalar(160, 120, 0, 0);
	tape_hsv_max = cvScalar(240, 255, 255, 0);
#else
	tape_hsv_min = cvScalar(70, 0, 0, 0);
	tape_hsv_max = cvScalar(130, 255, 255, 0);
#endif // _DEBUG

	
	element_size = 5;

	// 2014-8-25 22:41:21
	m_cvUndist8UC3_0 = cv::Mat::zeros (480,640,CV_8UC3);
	m_cvUndist8UC3_1 = cv::Mat::zeros (480,640,CV_8UC3);
	m_cvUndist32FC1_00 = cv::Mat::zeros (480,640,CV_32FC1);
	m_cvUndist32FC1_01 = cv::Mat::zeros (480,640,CV_32FC1);
	m_cvUndist32FC1_10 = cv::Mat::zeros (480,640,CV_32FC1);
	m_cvUndist32FC1_11 = cv::Mat::zeros (480,640,CV_32FC1);
	mapy_persp = cvCreateMat(480,640, CV_32FC1);
	mapx_persp = cvCreateMat(480,640, CV_32FC1);
}
// 
// FrameProcess::FrameProcess (cv::Mat mat)
// {
// 	m_yuv1downside = 95;
// 	m_yuv1upside  = 131;
// 	m_yuv2downside = 150;
// 	m_yuv2upside = 288;
// 	setProcessImg (mat);
// }


FrameProcess::~FrameProcess(void)
{
	if(m_pInstance)
		delete m_pInstance;
	m_pInstance = NULL;
	if(m_handInfos)
		delete [] m_handInfos;
	m_handInfos = NULL;
	if(m_pRawImgLeft)
		delete m_pRawImgLeft;
	m_pRawImgLeft = NULL;
	if(m_pRawImgRight)
		delete m_pRawImgRight;
	m_pRawImgRight = NULL;
}

// 通过对m_processImg进行肤色提取 设值成肤色部分 后期考虑从视差中得到深度 可去除背景部分
void FrameProcess::processSkinImg (int type)
{
	if(type | IMAGE_LEFT){
		for(int nY = 0; nY < m_cvProcessImgLeft.rows; nY++){
			for(int nX = 0; nX < m_cvProcessImgLeft.cols; nX++){
				if(m_step == 4){
					cv::Vec4b color = getColor4(m_cvProcessImgLeft,nX,nY);
					if(!isSkin(color)){
						//setColor(m_cvSkinImgLeft,nX,nY,cv::Vec4b(0,0,0,255));
						//setColor(m_cvSkinImgLeft,nX,nY,cv::Vec3b(0,0,0));
						// click手势中的色环也要加入clickleft/right
						uchar mask = ((uchar*)(m_circleMaskLeft.data + m_circleMaskLeft.step.p[0]*nY))[nX];
						if(mask > 0){
							setColor(m_cvSkinImgLeft,nX,nY,/*color*/cv::Vec3b(255,255,255));
							setColor(m_cvProcessClickLeft,nX,nY,color);
						}
						else{
							setColor(m_cvProcessClickLeft,nX,nY,cv::Vec4b(0,0,0,0));
							setColor(m_cvSkinImgLeft,nX,nY,cv::Vec3b(0,0,0));
						}
					}
					else{
						setColor(m_cvSkinImgLeft,nX,nY,/*color*/cv::Vec3b(255,255,255));
						setColor(m_cvProcessClickLeft,nX,nY,color);
					}
				}
				else if(m_step == 3){
					cv::Vec3b color = getColor3(m_cvProcessImgLeft,nX,nY);
					if(!isSkin(color)){
						//setColor(m_cvSkinImgLeft,nX,nY,cv::Vec4b(0,0,0,255));
						//setColor(m_cvSkinImgLeft,nX,nY,cv::Vec3b(0,0,0));
						// click手势中的色环也要加入clickleft/right
						uchar mask = ((uchar*)(m_circleMaskLeft.data + m_circleMaskLeft.step.p[0]*nY))[nX];
						if(mask > 0){
							setColor(m_cvSkinImgLeft,nX,nY,/*color*/cv::Vec3b(255,255,255));
							setColor(m_cvProcessClickLeft,nX,nY,color);
						}
						else{
							setColor(m_cvProcessClickLeft,nX,nY,cv::Vec3b(0,0,0));
							setColor(m_cvSkinImgLeft,nX,nY,cv::Vec3b(0,0,0));
						}
					}
					else{
						setColor(m_cvSkinImgLeft,nX,nY,/*color*/cv::Vec3b(255,255,255));
						setColor(m_cvProcessClickLeft,nX,nY,color);
					}
				}
			}
		}
		//imshow ("m_cvProcessClickLeft",m_cvProcessClickLeft);
#ifdef _DEBUG
		imwrite ("skinImgLeft.jpg",m_cvSkinImgLeft);
		//imwrite ("m_cvProcessClickLeft.png",m_cvProcessClickLeft);
#endif // DEBUG
	}
	if(type | IMAGE_RIGHT){
		for(int nY = 0; nY < m_cvProcessImgRight.rows; nY++){
			for(int nX = 0; nX < m_cvProcessImgRight.cols; nX++){
				if(m_step == 4){
					cv::Vec4b color = getColor4(m_cvProcessImgRight,nX,nY);
					if(!isSkin(color)){
						//setColor(m_cvSkinImgRight,nX,nY,cv::Vec4b(0,0,0,255));
							/*setColor(m_cvSkinImgRight,nX,nY,cv::Vec3b(0,0,0));*/
							// click手势中的色环也要加入clickleft/right
							uchar mask = ((uchar*)(m_circleMaskRight.data + m_circleMaskRight.step.p[0]*nY))[nX];
							if(mask > 0){
								setColor(m_cvProcessClickRight,nX,nY,color);
								// 对色环区域也附上颜色
								setColor(m_cvSkinImgRight,nX,nY,/*color*/cv::Vec3b(255,255,255));
							}
							else{
								setColor(m_cvProcessClickRight,nX,nY,cv::Vec4b(0,0,0,0));
								setColor(m_cvSkinImgRight,nX,nY,cv::Vec3b(0,0,0));
							}
					}
					else{
						setColor(m_cvSkinImgRight,nX,nY,/*color*/cv::Vec3b(255,255,255));
						setColor(m_cvProcessClickRight,nX,nY,color);
					}
				}
				else if(m_step == 3){
					cv::Vec3b color = getColor3(m_cvProcessImgRight,nX,nY);
					if(!isSkin(color)){
						// click手势中的色环也要加入clickleft/right
						uchar mask = ((uchar*)(m_circleMaskRight.data + m_circleMaskRight.step.p[0]*nY))[nX];
						if(mask > 0){
							setColor(m_cvSkinImgRight,nX,nY,/*color*/cv::Vec3b(255,255,255));
							setColor(m_cvProcessClickRight,nX,nY,color);
						}
						else{
							setColor(m_cvProcessClickRight,nX,nY,cv::Vec3b(0,0,0));
							setColor(m_cvSkinImgRight,nX,nY,cv::Vec3b(0,0,0));
						}
					}
					else{
						setColor(m_cvSkinImgRight,nX,nY,/*color*/cv::Vec3b(255,255,255));
						setColor(m_cvProcessClickRight,nX,nY,color);
					}
				}
			}
		}
#ifdef _DEBUG
		imwrite ("skinImgRight.jpg",m_cvSkinImgRight);
#endif // DEBUG
	}
}

int FrameProcess::setYuv (YUV yuvPara,int value)
{
	switch(yuvPara){
	case YUV1DOWN:
		m_yuv1downside += value;
		return m_yuv1downside;
		break;
	case YUV1UP:
		m_yuv1upside += value;
		return m_yuv1upside;
		break;
	case YUV2DOWN:
		m_yuv2downside += value;
		return m_yuv2downside;
		break;
	case YUV2UP:
		m_yuv2upside += value;
		return m_yuv2upside;
		break;
	default:
		break;
	}
	return 0;
}

FrameProcess* FrameProcess::getInstance ()
{
	if(m_pInstance == NULL)
		m_pInstance = new FrameProcess;
	return m_pInstance;
}

void FrameProcess::getAction (int type, 
							  ARGlassAction* menuActions, int* menuActionCnt, 
							  ARGlassAction* clickActions, int* clickActionCnt)
{
	std::fstream outputfile;
	outputfile.open ("log.txt",std::ios_base::app);
	menuActionCnt[0] = 0;
	clickActionCnt[0] = 0;
	// step one: check fingers 
	menuActions[0].name = menuActions[1].name = ACTION::NONE;

	if(type | IMAGE_RIGHT){
		getFingers(m_cvSkinImgRight,m_cvMaxContourRight,m_handInfos);
		for(int i = 0; i < 2; i++){
			if(m_handInfos[i].nfingers >= 4){			// menu action
				menuActions[i].name = ACTION::MENU;
				menuActions[i].pos = m_handInfos[i].palmPos;
				++menuActionCnt[0];
			}
// 			else if(m_handInfos[i].nfingers == 1){
// 				clickActions[i].name = ACTION::CLICK;
// 				clickActions[i].pos = m_handInfos[i].fingerPos[0];
// 				outputfile << "opencv found CLICK:\t" << clickActions[i].pos << "\n";
// 			}
		}
	}

	if(type | IMAGE_LEFT){
		getFingers(m_cvSkinImgLeft,m_cvMaxContourLeft,m_handInfos);
		for(int i = 0; i < 2; i++){
			if(m_handInfos[i].nfingers >= 4){			// menu action
				menuActions[i].name = ACTION::MENU;
				menuActions[i].pos = m_handInfos[i].palmPos;
			}
// 			else if(m_handInfos[i].nfingers == 1){
// 				clickActions[i].name = ACTION::CLICK;
// 				clickActions[i].pos = m_handInfos[i].fingerPos[0];
// 				outputfile << "opencv found CLICK:\t" << clickActions[i].pos << "\n";
// 			}
		}		
	}

	// hehe 2014-5-13 13:41:23
	// 绘制click手势的色图
	// 2014-5-15 16:00:58
	//imshow ("m_cvMaxContourLeft0",m_cvMaxContourLeft[0]);
	//imshow ("m_cvMaxContourLeft1",m_cvMaxContourLeft[1]);
	drawClickFinger ();

	// 把指环检测结果放入click
	clickActionCnt[0] = 0;
	if(m_fingerTipsLeftFinal.size () == m_fingerTipsRightFinal.size ()){
		std::map<float,float>::iterator itBegin = m_fingerTipsLeftFinal.begin ();	
		for(int i = 0;(itBegin!= m_fingerTipsLeftFinal.end ()) && (i < 2); itBegin++, i++){
			clickActions[i].name = ACTION::CLICK;
			clickActions[i].pos = cv::Point2f(itBegin->first,itBegin->second);
			++clickActionCnt[0];
			outputfile << "opencv found circle CLICK:\t" << clickActions[i].pos << "\n";
		}
	}
	else{
		outputfile << "opencv found circle CLICK but different size with double hands:\n";
	}
	outputfile.close ();
}

// 老招数 先找个轮廓 近似个多边形 然后手指提取 返回手指数目
void FrameProcess::getFingers (cv::Mat &img, cv::Mat & maxContour, HandInfo * hands ,cv::Mat &processOriginImg,cv::Mat & processClickImg)
{
	// get contour
	
	//m_cvContourDst = cv::Mat::zeros(img.rows,img.cols,CV_8UC3);
	//做开闭降噪
	morphologyEx (img, img, cv::MORPH_CLOSE, element);
	//imshow ("skinAfterClose",img);
	morphologyEx (img, img, cv::MORPH_OPEN, element);
	//imshow ("skinAfterOpen",img);

	// get biggest contour which is hand contour
	findContour (img,cvHandContourPoints,maxContour,m_cvContourDst,processOriginImg,processClickImg);
	//getMaxContour(cvHandContourPoints,m_cvContourDst,maxContourIdx);
	// hehe 2014-5-13 13:41:23
	// 绘制click手势的色图
	//drawClickFinger ();

	// 近似多边形看手指 img只用来绘制手指 不做输入 所以不分开处理了
	//int fingerNum = 0;
	if(cvHandContourPoints.size () > maxContourIdx)
		hands[0].nfingers = approxFinger(cvHandContourPoints[maxContourIdx],img,hands[0].palmPos,hands[0].fingerPos);
	if((cvHandContourPoints.size () > maxContourIdx2)
		&&(maxContourIdx2 != -1))
		hands[1].nfingers = approxFinger(cvHandContourPoints[maxContourIdx2],img,hands[1].palmPos,hands[1].fingerPos);
#ifdef _DEBUG
	//imwrite ("fingers.jpg",img);
#endif // _DEBUG

	//img += m_cvFingers;
}

void FrameProcess::getFingers (cv::Mat &img, cv::Mat * maxContour, HandInfo * hands )
{
	// get contour

	//m_cvContourDst = cv::Mat::zeros(img.rows,img.cols,CV_8UC3);
	//做开闭降噪
	morphologyEx (img, img, cv::MORPH_CLOSE, element);
	//imwrite ("skinAfterClose.jpg",img);
	morphologyEx (img, img, cv::MORPH_OPEN, element);
	//imshow ("skinAfterOpen.jpg",img);

	// get biggest contour which is hand contour
	findContour (img,cvHandContourPoints,maxContour,m_cvContourDst);
	//getMaxContour(cvHandContourPoints,m_cvContourDst,maxContourIdx);

	// 近似多边形看手指 img只用来绘制手指 不做输入 所以不分开处理了
	//int fingerNum = 0;
	if(cvHandContourPoints.size () > maxContourIdx)
		hands[0].nfingers = approxFinger(cvHandContourPoints[maxContourIdx],img,hands[0].palmPos,hands[0].fingerPos);
	if((cvHandContourPoints.size () > maxContourIdx2)
		&&(maxContourIdx2 != -1))
		hands[1].nfingers = approxFinger(cvHandContourPoints[maxContourIdx2],img,hands[1].palmPos,hands[1].fingerPos);
#ifdef _DEBUG
	//imwrite ("fingers.jpg",img);
#endif // _DEBUG
}

void FrameProcess::findContour (cv::Mat src,std::vector<std::vector<cv::Point> > &contours , cv::Mat& maxContour,cv::Mat &dst ,cv::Mat & processOriginImg,cv::Mat& processClickImg)
{
	//dst = cv::Mat::zeros(src.rows,src.cols,CV_8UC1);
	//dst = src.clone ();
#ifdef _DEBUG
	//imwrite ("initial.jpg",src);
#endif // DEBUG
	if(m_step == 3){
		cvtColor (src,dst,CV_RGB2GRAY);
	}
	else if(m_step == 4){
		cvtColor (src,dst,CV_RGBA2GRAY);
	}
#ifdef _DEBUG
	//imwrite ("convertTogray.jpg",dst);
#endif // DEBUG
	dst.convertTo(dst,CV_8UC1);
#ifdef _DEBUG
	//imwrite ("convertTo8UC1.jpg",dst);
#endif // DEBUG
	// 2012-04-11 16:53 changed
	findContours( dst, contours, hierarchy, CV_RETR_EXTERNAL/*RETR_TREE*/, cv::CHAIN_APPROX_SIMPLE);

	// 反正下头要再画个最大轮廓 这里就不画了
	drawLevel = 1;
	drawContours( dst, contours, -1, POWER_2_16-1,3, CV_AA, hierarchy, drawLevel);
#ifdef _DEBUG
	//imwrite ("contour.jpg",dst);
#endif // DEBUG
	getMaxContour (contours);
	if(contours.size () > maxContourIdx){
		drawContours( maxContour, contours, maxContourIdx, POWER_2_8-1,CV_FILLED, CV_AA/*, hierarchy, drawLevel*/);
		// 2014-5-15 14:55:16 增加判断 clickfinger是否在maxContour中
		std::map<float,float>::iterator itBegin = m_fingerTipsLeft.begin ();
		for(;(itBegin!= m_fingerTipsLeft.end ()); itBegin++){
			if(((uchar*)(maxContour.data + maxContour.step.p[0]* (int)itBegin->second))[(int)itBegin->first] > 0){
				// 2014-5-15 14:48:24 在这里判断哪个轮廓含有click手势那个点 就把他copyto clickfinger
				drawClickFingerRevised (maxContour,processOriginImg,processClickImg);
				break;
			}
		}
	}
	if((contours.size() > maxContourIdx2)
		&& (maxContourIdx2 != -1)){
		drawContours( maxContour, contours, maxContourIdx2, POWER_2_8-1,CV_FILLED, CV_AA/*, hierarchy, drawLevel*/);
		// 2014-5-15 14:55:16 增加判断 clickfinger是否在maxContour中
		std::map<float,float>::iterator itBegin = m_fingerTipsLeft.begin ();
		for(;(itBegin!= m_fingerTipsLeft.end ()); itBegin++){
			if(((uchar*)(maxContour.data + maxContour.step.p[0]* (int)itBegin->second))[(int)itBegin->first] > 0){
				// 2014-5-15 14:48:24 在这里判断哪个轮廓含有click手势那个点 就把他copyto clickfinger
				drawClickFingerRevised (contours,maxContourIdx2,processOriginImg,processClickImg);
				break;
			}
		}
	}
#ifdef _DEBUG
	//imwrite ("maxContour.jpg",m_cvMaxContour);
#endif // DEBUG
}

void FrameProcess::findContour (cv::Mat src,std::vector<std::vector<cv::Point> > &contours , cv::Mat* maxContour,cv::Mat &dst)
{
#ifdef _DEBUG
	//imwrite ("initial.jpg",src);
#endif // DEBUG
	if(m_step == 3){
		cvtColor (src,dst,CV_RGB2GRAY);
	}
	else if(m_step == 4){
		cvtColor (src,dst,CV_RGBA2GRAY);
	}
#ifdef _DEBUG
	//imwrite ("convertTogray.jpg",dst);
#endif // DEBUG
	dst.convertTo(dst,CV_8UC1);
#ifdef _DEBUG
	//imwrite ("convertTo8UC1.jpg",dst);
#endif // DEBUG
	// 2012-04-11 16:53 changed
	findContours( dst, contours, hierarchy, CV_RETR_EXTERNAL/*RETR_TREE*/, cv::CHAIN_APPROX_SIMPLE);

	// 反正下头要再画个最大轮廓 这里就不画了
	drawLevel = 1;
	drawContours( dst, contours, -1, POWER_2_16-1,3, CV_AA, hierarchy, drawLevel);
#ifdef _DEBUG
	//imwrite ("contour.jpg",dst);
#endif // DEBUG
	getMaxContour (contours);
	if(contours.size () > maxContourIdx){		
		drawContours( maxContour[0], contours, maxContourIdx, POWER_2_8-1,CV_FILLED, CV_AA/*, hierarchy, drawLevel*/);
	}
	if((contours.size() > maxContourIdx2)
		&& (maxContourIdx2 != -1)){
			drawContours( maxContour[1], contours, maxContourIdx2, POWER_2_8-1,CV_FILLED, CV_AA/*, hierarchy, drawLevel*/);
	}
#ifdef _DEBUG
	imwrite ("maxContour0.jpg",maxContour[0]);
	imwrite ("maxContour1.jpg",maxContour[1]);
#endif // DEBUG
}

int FrameProcess::getMaxContour (std::vector<std::vector<cv::Point> > cvHandContourPoints)
{
	// 2014-5-6 21:13:25
	// get the max 2 contour 因为会有两只手呀
	std::vector<cv::Point> maxContour1, maxContour2;
	double maxContourSize1 = 0, maxContourSize2 = 0;

	for(int i = 0; i < cvHandContourPoints.size(); i++){
		double contourSize = contourArea(cvHandContourPoints[i]);
		if(contourSize > maxContourSize1){
			// 来了个最大的 最大移到二大 新的给最大
			maxContour2 = maxContour1;
			maxContourSize2 = maxContourSize1;
			maxContourIdx2 = maxContourIdx;

			maxContour1 = cvHandContourPoints[i];	
			maxContourSize1 = contourSize;
			maxContourIdx = i;
		}else if(contourSize > maxContourSize2){
			// 来了个次大的 赋值给次大
			maxContour2 = cvHandContourPoints[i];	
			maxContourSize2 = contourSize;
			maxContourIdx2 = i;
		}
	}
	// 判断 如果次大还没有最大三分之一大 那就翟建
	if(maxContourSize2 < (maxContourSize1 / 3.0))
		maxContourIdx2 = -1;
	// draw contours to make new img
	//cvContourDst = cv::Mat::zeros(cvContourDst.rows,cvContourDst.cols,cvContourDst.type());
// 	cv::Scalar color(100,100,100);
// 	drawContours(cvContourDst,cvHandContourPoints,maxContourIdx,color,CV_FILLED);	
// 	imwrite ("maxContour.jpg",cvContourDst);
 	return maxContourIdx;
}

int FrameProcess::approxFinger (std::vector<cv::Point > cvHandContourPoints, cv::Mat &cvFingers, 
								cv::Point &palmPos, cv::Point* fingerPos)
{	 
	//cvFingers = cv::Mat::zeros(m_processImgLeft.cols,m_processImgLeft.rows,m_processImgLeft.type ());
	//handCount = 0;

	cv::Point2i fingers[SINGLER_FINGER_NUMBER*2][HAND_NUMBER];
	memset(fingers,0,sizeof(cv::Point2i)*SINGLER_FINGER_NUMBER*2*HAND_NUMBER);
	float fingerAngle[SINGLER_FINGER_NUMBER];
	int fingersIdx = 0,fingerAngleIdx = 0;
	cv::Rect contourRect;

	//TODO 后期验证 多大的大小为标准
	//if(cv::contourArea(cvHandContourPoints) < 100)
//		return -1;
	// 根据轮廓数组进行多边形拟合
	cv::Mat cvMatHandContourPoints = cv::Mat(cvHandContourPoints);
	approxPolyDP(cvMatHandContourPoints, approx, arcLength(cvMatHandContourPoints, true)*epsilon, isClosed);
	contourRect = boundingRect(approx);
	palmPos = cv::Point(contourRect.x + contourRect.width/2 - m_width/2,
		m_height/2 - contourRect.y + contourRect.height/2);
	
#ifdef _DEBUG
	//// Iterate over each segment and draw it  
	std::vector<cv::Point>::const_iterator itp= cvHandContourPoints.begin();  
	while (itp!=(cvHandContourPoints.end()-1)) {  
		line(m_cvApprox,*itp,*(itp+1),cv::Scalar(120),2);  
		++itp;  
	}  
	//imwrite ("approx.jpg",m_cvApprox);
	//// Iterate over each segment and draw it  
	itp= approx.begin();  
	while (itp!=(approx.end()-1)) {  
		line(m_cvApprox,*itp,*(itp+1),cv::Scalar(255),5);  
		++itp;  
	}  
	//imwrite ("approx.jpg",m_cvApprox);
#endif // _DEBUG
	
	for(int j = 0; j < approx.size();){
		// check the angle, if smaller than threshold 
		cv::Point p1 = approx[j],p2 = approx[(j+1)%approx.size()],p3 = approx[(j+2)%approx.size()];
		double cosine = angle(p1, p2, p3);
		cv::Point pcenter = cv::Point((p1.x + p2.x + p3.x)/3,(p1.y + p2.y + p3.y)/3);
		// judge if it is in contour
		if(pointPolygonTest(approx,pcenter, 0 ) >= 0){
			// judge if it is smaller than 45 angle
			if(cosine >= COS_FINGERTIP){
				cv::Point middleP(	(p1.x + p3.x)/2,(p1.y + p3.y)/2	);
				// judge if it is longer than
				
				if(	fabs(sqrt(dis(p2 , middleP)))	> 
					((contourRect.width + contourRect.height)/2 * FINGER_PERCENT)	){
						// really finger
						if(fingerAngleIdx == SINGLER_FINGER_NUMBER)
							return 0;
						fingers[fingersIdx][0] = p2;
						// 2014-5-9 14:45:04 hehe
						fingerPos[fingersIdx].x = p2.x;
						fingerPos[fingersIdx].y = m_height - p2.y;

						fingers[fingersIdx][1] = middleP;
						++fingersIdx;
						fingerAngle[fingerAngleIdx++] = cosine;
						
						// draw finger
						if(m_step == 3)
							line(cvFingers,p2,middleP,cv::Scalar(100,100,100),3,CV_AA,0);
						else if(m_step == 4)
							line(cvFingers,p2,middleP,cv::Scalar(100,100,100,255),3,CV_AA,0);
						j+=2;
						continue;
				}
			}
		}
		++j;
	}
	return fingersIdx;
}

void FrameProcess::calibLeftImage ()
{
#ifdef _DEBUG
	//imwrite ("beforeTrans.jpg",cv::Mat(m_height,m_width,m_cvProcessImgRight.type (),m_pRawImgLeft));
#endif // DEBUG
	// 坐标变换
	double nXtemp,nYtemp;
	for(int nY = 0; nY < m_height; nY++){
		for(int nX = 0; nX < m_width; nX++){
			if(m_step == 4){
				nXtemp = nX;
				nYtemp = nY;
				transferCoord (nXtemp,nYtemp,m_width/2,m_height/2);
				cv::Vec4b color = getColor4 (m_pRawImgLeft,nXtemp,nYtemp);
				setColor(m_cvProcessImgLeft,nX,nY,color);
			}
			else if(m_step == 3){
				nXtemp = nX;
				nYtemp = nY;
				transferCoord (nXtemp,nYtemp,m_width/2,m_height/2);
				cv::Vec3b color = getColor3 (m_pRawImgLeft,nXtemp,nYtemp);
				setColor(m_cvProcessImgLeft,nX,nY,color);
			}
		}
	}
#ifdef _DEBUG
	//imwrite ("afterTrans.jpg",m_cvProcessImgLeft);
#endif // DEBUG
}


void FrameProcess::setRawImg (uchar* val1,uchar* val2)
{
	//m_pRawImgLeft = val1;
	//m_pRawImgRight = val2;
	int type = (m_step == 3)? CV_8UC3 : CV_8UC4;
	m_cvProcessImgRight = cv::Mat(m_height,m_width,type,val2);
	if(!m_bInitSkin){
		m_cvSkinImgLeft = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC3);
		m_cvSkinImgRight = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC3);
		m_hsvImgLeft = cv::Mat::zeros(m_cvProcessImgLeft.size(),CV_8UC3);
		m_hsvImgRight = cv::Mat::zeros(m_cvProcessImgLeft.size(),CV_8UC3);
		m_circleMaskLeft = cv::Mat::zeros(m_cvProcessImgLeft.size (), CV_8UC1);
		m_circleMaskRight = cv::Mat::zeros(m_cvProcessImgLeft.size (), CV_8UC1);
		m_circleMaskHelp = cv::Mat::zeros(m_cvProcessImgLeft.size (), CV_8UC1);
		m_cvProcessClickLeft =  cv::Mat::zeros(m_height,m_width, type);
		m_cvProcessClickRight =  cv::Mat::zeros(m_height,m_width, type);
		element = getStructuringElement (cv::MORPH_ELLIPSE, cv::Size (element_size, element_size));
		m_pRawImgLeft = new uchar(m_height * m_width * m_step);
		m_cvContourDst = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC1);
		// 2014-8-25 22:41:51 end
		m_bInitSkin = true;
	}
	for(int i = 0; i < 2; i++){
		m_cvMaxContourLeft[i] = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC1);
		m_cvMaxContourRight[i] = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC1);
	}	
// 	imshow ("m_cvMaxContourLeftInit0",m_cvMaxContourLeft[0]);
// 	imshow ("m_cvMaxContourLeftInit1",m_cvMaxContourLeft[1]);
	m_cvMaxContourTemp = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC1);
	m_cvApprox = cv::Mat::zeros(m_height,m_width,/*type*/CV_8UC1);
	
	//std::fstream outputfile;
	//outputfile.open ("log.txt",std::ios_base::app);
	//outputfile << "val1:\t" << (int)val1 << "\tval2:\t" << (int)val2 << "\n";
	//outputfile.close ();
	// do calib
	if(!m_bPostCalib){		
		memcpy_s (m_pRawImgLeft,m_height * m_width * m_step,val1,m_height * m_width * m_step);
		m_cvProcessImgLeft = cv::Mat::zeros(m_height,m_width,type);
		calibLeftImage ();	
	}else{
		m_cvProcessImgLeft = cv::Mat(m_height,m_width,type,val1);
	}
	m_fingerTipsLeft.clear ();
	m_fingerTipsRight.clear ();
	m_fingerTipsLeftFinal.clear ();
	m_fingerTipsRightFinal.clear ();
	maxContourIdx = -1;
	maxContourIdx2 = -1;
	clickContourIdx = -1;
}

void FrameProcess::createCalibrationMatrix ()
{
	// 实测 是逆时针方向旋转
	for(int nY = 0; nY < m_cvExRotateMatrix.rows; nY++){
		for(int nX = 0; nX < m_cvExRotateMatrix.cols; nX++){
			m_rotateMatrix[nX][nY] = m_cvExRotateMatrix.at<double>(nY,nX);
		}
	}
	m_cvExRotateMatrixInv = m_cvExRotateMatrix.inv ();
	for(int nY = 0; nY < m_cvInM1.rows; nY++){
		for(int nX = 0; nX < m_cvInM1.cols; nX++){
			m_intrinsics[nX][nY] = m_cvInM1.at<double>(nY,nX);
		}
	}	
	m_cvInM1Inv = m_cvInM1.inv ();
// 	double alpha = /*0;*/3.1415926/18;
// 	double cosa = cos(alpha);
// 	double sina = sin(alpha);
// 	m_rotateMatrix [0][0] = cosa;
// 	m_rotateMatrix [1][0] = -sina;
// 	m_rotateMatrix [2][0] = 0;
// 	m_rotateMatrix [0][1] = sina;
// 	m_rotateMatrix [1][1] = cosa;
// 	m_rotateMatrix [2][1] = 0;
// 	m_rotateMatrix [0][2] = 0;
// 	m_rotateMatrix [1][2] = 0;
// 	m_rotateMatrix [2][2] = 1;

// 	double alpha = /*0;*/3.1415926/18;
// 	double cosa = cos(alpha);
// 	double sina = sin(alpha);
// 	m_rotateMatrix [0][0] = 1;
// 	m_rotateMatrix [1][0] = 0;
// 	m_rotateMatrix [2][0] = 0;
// 	m_rotateMatrix [0][1] = 0;
// 	m_rotateMatrix [1][1] = cosa;
// 	m_rotateMatrix [2][1] = -sina;
// 	m_rotateMatrix [0][2] = 0;
// 	m_rotateMatrix [1][2] = sina;
// 	m_rotateMatrix [2][2] = cosa;

// 	double alpha = /*0;*/-3.1415926/18;
// 	double cosa = cos(alpha);
// 	double sina = sin(alpha);
// 	m_rotateMatrix [0][0] = cosa;
// 	m_rotateMatrix [1][0] = 0;
// 	m_rotateMatrix [2][0] = -sina;
// 	m_rotateMatrix [0][1] = 0;
// 	m_rotateMatrix [1][1] = 1;
// 	m_rotateMatrix [2][1] = 0;
// 	m_rotateMatrix [0][2] = sina;
// 	m_rotateMatrix [1][2] = 0;
// 	m_rotateMatrix [2][2] = cosa;

	m_transMatrix = glm::vec3(/*(float)-3.5040969434357035 * (float)0.01*/0, (float)-5.8703667251524720,
		/*(float)7.2664250232939778 * (float)0.01*/0);
}

void FrameProcess::stereoMatch (cv::Mat& cvLeft,cv::Mat& cvRight)
{
	// step 1 文件输入参数 这个应该放在初始化的地方
	
	// step 2 进行立体匹配 校正图像 获取视差
	cv::stereoRectify (m_cvInM1,m_cvInD1,m_cvInM2,m_cvInD2,cv::Size(m_width,m_height),
		m_cvExRotateMatrix,m_cvExTransMatrix,
		m_cvR1,m_cvR2,m_cvP1,m_cvP2,m_cvQ
		// TODO: 疑问 为何我加了两个roi 感觉后来矫正的原图都变了
		,cv::CALIB_ZERO_DISPARITY,-1,cv::Size(m_width,m_height),&m_rectRoi1,&m_rectRoi2
		);
	initUndistortRectifyMap (m_cvInM1,m_cvInD1,m_cvR1,m_cvP1,cv::Size(m_width,m_height),CV_16SC2,map11,map12);
	initUndistortRectifyMap (m_cvInM2,m_cvInD2,m_cvR2,m_cvP2,cv::Size(m_width,m_height),CV_16SC2,map21,map22);
	
	
// 	remap(m_cvProcessImgLeft, img1r, map11, map12, cv::INTER_LINEAR);
// 	remap(m_cvProcessImgRight, img2r, map21, map22, cv::INTER_LINEAR);
	// test for left and right exchange
	remap(cvLeft, img1r, map11, map12, cv::INTER_LINEAR);
	remap(cvRight, img2r, map21, map22, cv::INTER_LINEAR);

	//imwrite ("before_rectify_left.jpg",cvLeft);

#ifdef _DEBUG
	//imwrite ("before_rectify_left.jpg",cvLeft);
	//imwrite ("before_rectify_right.jpg",cvRight);
#endif // DEBUG
	cvLeft = img1r;
	cvRight = img2r;
#ifdef _DEBUG
	// 突然发现校正图像会自己给我做旋转 机智的想哭 不如试试看咯 一切为了速度
	//imwrite ("after_rectify_left.jpg",cvLeft);
	//imwrite ("after_rectify_right.jpg",cvRight);
#endif // DEBUG
	// step 3 相关输出
	//imwrite ("after_rectify_left.jpg",cvRight);
	//imwrite ("after_rectify_left.jpg",cvLeft);
	//imwrite ("after_rectify_right.jpg",cvRight);
}

void FrameProcess::stereoMatch ()
{
	// rectify images
	if(!m_bStereoRectify)
		return;
	stereoMatch (m_cvProcessImgRight,m_cvProcessImgLeft);
}

void FrameProcess::getIntrExtrinsicInput ()
{
	fs.open("extrinsics.yml", cv::FileStorage::READ);
	if( fs.isOpened() ){
		fs["R"] >> m_cvExRotateMatrix;
		fs["T"] >> m_cvExTransMatrix;
	}
	fs.release ();
	m_cvExRotateMatrixInv = m_cvExRotateMatrix.inv ();
	fs.open("intrinsics.yml", cv::FileStorage::READ);
	if( fs.isOpened() ){
		fs["M1"] >> m_cvInM1;
		fs["M2"] >> m_cvInM2;
		fs["D1"] >> m_cvInD1;
		fs["D2"] >> m_cvInD2;
	}
	fs.release ();
	m_cvInM1Inv = m_cvInM1.inv ();
}

void FrameProcess::processDisparity (cv::Mat& cvLeft, cv::Mat& cvRight, cv::string name)
{
#ifdef _DEBUG
	//imwrite (name + "proDisLeft.jpg",cvLeft);
	//imwrite (name + "proDisRight.jpg",cvRight);
#endif // _DEBUG
	if( m_alg == STEREO_BM )
		bm(cvLeft, cvRight, disp);
	else if( m_alg == STEREO_VAR ) {
		var(cvLeft, cvRight, disp);
	}
	else if( m_alg == STEREO_SGBM || m_alg == STEREO_HH )
		sgbm(cvLeft, cvRight, disp);
		// 让老夫换一个试试
		//sgbm(m_cvProcessImgLeft, m_cvProcessImgRight, disp);
	if( m_alg != STEREO_VAR )
		disp.convertTo(disp8, CV_8U, 255/(m_numberOfDisparities*16.));
	else
		disp.convertTo(disp8, CV_8U);
	// 反投影回三维暂且不用 接下来要做实验 这个深度究竟在多少 定一个阈值 然后超过阈值的就不予理睬 增加肤色提取的稳定度
#ifdef _DEBUG
	//imwrite(name + "disparity.jpg", disp8);
#endif // _DEBUG
}

void FrameProcess::processDisparity ()
{
	// 对肤色图进行视差提取
	processDisparity (m_cvSkinImgLeft,m_cvSkinImgRight,"skin");
	// 对原图进行立体匹配视察提取
	processDisparity (m_cvProcessImgLeft,m_cvProcessImgRight,"original");
}

void FrameProcess::createStereoPara ()
{
	m_alg = STEREO_SGBM;
	m_numberOfDisparities = 96;
	m_uniquenessRatio = 20;

	bm.state->roi1 = m_rectRoi1;
	bm.state->roi2 = m_rectRoi2;
	bm.state->preFilterCap = 13;//31;//13
	bm.state->SADWindowSize = 21;//9;//19
	bm.state->minDisparity = 0;
	bm.state->numberOfDisparities = m_numberOfDisparities;
	bm.state->textureThreshold = 10;
	bm.state->uniquenessRatio = m_uniquenessRatio;//15;//20
	bm.state->speckleWindowSize = 100;//13
	bm.state->speckleRange = 32;
	bm.state->disp12MaxDiff = 1;
	bm.state->preFilterCap = 13;

	sgbm.preFilterCap = 63;
	sgbm.SADWindowSize = 3;

	int cn = m_cvProcessImgLeft.channels();
	
	sgbm.P1 = 8*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.P2 = 32*cn*sgbm.SADWindowSize*sgbm.SADWindowSize;
	sgbm.minDisparity = 0;
	sgbm.numberOfDisparities = m_numberOfDisparities;
	sgbm.uniquenessRatio = m_uniquenessRatio;
	sgbm.speckleWindowSize = bm.state->speckleWindowSize;
	sgbm.speckleRange = bm.state->speckleRange;
	sgbm.disp12MaxDiff = 1;
	sgbm.fullDP = m_alg == STEREO_HH;

	var.levels = 3;                                 // ignored with USE_AUTO_PARAMS
	var.pyrScale = 0.5;                             // ignored with USE_AUTO_PARAMS
	var.nIt = 25;
	var.minDisp = -m_numberOfDisparities;
	var.maxDisp = 0;
	var.poly_n = 3;
	var.poly_sigma = 0.0;
	var.fi = 15.0f;
	var.lambda = 0.03f;
	var.penalization = var.PENALIZATION_TICHONOV;   // ignored with USE_AUTO_PARAMS
	var.cycle = var.CYCLE_V;                        // ignored with USE_AUTO_PARAMS
	var.flags = var.USE_SMART_ID | var.USE_AUTO_PARAMS | var.USE_INITIAL_DISPARITY | var.USE_MEDIAN_FILTERING ;
}

void FrameProcess::setDetail (int height,int width,int step,bool isPostCalib,bool isStereoRectify)
{
	m_step = step;
	m_height = height;
	m_width = width;
	m_bPostCalib = isPostCalib;
	m_bStereoRectify = isStereoRectify;
}

// unity里头得80-160才好 好奇怪哟 记得改

void FrameProcess::processFingerCircle ()
{	
	//imwrite ("m_cvProcessImgRightBeforeprocessFingerCircle.jpg",m_cvProcessImgRight);
	// test rgb2hsv bgr2hsv 感觉差不多是怎么回事= =
	cvtColor (m_cvProcessImgLeft,m_hsvImgLeft,CV_RGB2HSV);
	cvtColor (m_cvProcessImgRight,m_hsvImgRight,CV_RGB2HSV);
#ifdef _DEBUG
	//imwrite ("rgba2rgb2hsv.jpg",m_hsvImgLeft);
	//imshow ("rgba2rgb2hsv",m_hsvImgLeft);
#endif // _DEBUG	
// 
// 	cvtColor (m_cvProcessImgLeft,m_hsvImg,CV_RGBA2RGB);
// 	cvtColor (m_hsvImg,m_hsvImg,CV_RGB2HSV);

// 	cvtColor (m_cvProcessImgLeft,hsvTest1,CV_RGB2HSV);
// 	imwrite ("rgb2hsv.jpg",hsvTest1);
// 	imshow ("rgb2hsv",hsvTest1);
// 
// 	cvtColor (m_cvProcessImgLeft,hsvTest1,CV_BGRA2RGB);
// 	cvtColor (hsvTest1,hsvTest1,CV_RGB2HSV);
// 	imwrite ("bgra2rgb2hsv.jpg",hsvTest1);
// 	imshow ("bgra2rgb2hsv",hsvTest1);
// 
// 	cvtColor (m_cvProcessImgLeft,hsvTest1,CV_BGR2HSV);
// 	imwrite ("bgr2hsv.jpg",hsvTest1);
// 	imshow ("bgr2hsv",hsvTest1);

	// 色环提取
	inRange (m_hsvImgLeft, tape_hsv_min, tape_hsv_max, m_circleMaskLeft);
	inRange (m_hsvImgRight, tape_hsv_min, tape_hsv_max, m_circleMaskRight);

	// 降噪
	morphologyEx (m_circleMaskLeft, m_circleMaskLeft, cv::MORPH_OPEN, element);
	morphologyEx (m_circleMaskLeft, m_circleMaskLeft, cv::MORPH_CLOSE, element);
	morphologyEx (m_circleMaskRight, m_circleMaskRight, cv::MORPH_OPEN, element);
	morphologyEx (m_circleMaskRight, m_circleMaskRight, cv::MORPH_CLOSE, element);
	//imshow ("after-denoise", m_circleMaskLeft);
#ifdef _DEBUG
	
	//imwrite ("afterDenoiseCircle.jpg",m_circleMaskLeft);
#endif // _DEBUG	

	// 连通域筛选	
	m_circleMaskHelp = m_circleMaskLeft.clone ();
	findContours (m_circleMaskHelp, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < contours.size (); ++i)	{
		// 600实验数据→1000
		if (contourArea (contours[i]) > 200){
			// 获取最小包围圆
			minEnclosingCircle (contours[i], center, radius);
			//drawContours (src, contours, i, cv::Scalar (255), CV_FILLED);
			circle (m_hsvImgLeft, center, 3, cv::Scalar (127), CV_FILLED);

			// 记录检测到指尖色环点
			m_fingerTipsLeft.insert (std::make_pair (center.x,center.y));
		}
	}
	imshow ("afterCircle.jpg",m_hsvImgLeft);

	//m_circleMaskHelp = m_circleMaskRight.clone ();
	findContours (/*m_circleMaskHelp*/m_circleMaskRight, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	for (size_t i = 0; i < contours.size (); ++i)	{
		// 600实验数据→300→200
		std::fstream outputfile;
		outputfile.open ("log.txt",std::ios_base::app);
		outputfile << "contourArea:\t" << contourArea (contours[i]) << "\n";
		outputfile.close ();
		if (contourArea (contours[i]) > 200){
			// 获取最小包围圆
			minEnclosingCircle (contours[i], center, radius);
			//drawContours (src, contours, i, cv::Scalar (255), CV_FILLED);
			circle (m_hsvImgRight, center, 3, cv::Scalar (127), CV_FILLED);

			// 记录检测到指尖色环点
			m_fingerTipsRight.insert (std::make_pair (center.x,center.y));
		}
	}
#ifdef _DEBUG	
	//imwrite ("afterCircle.jpg",m_hsvImgLeft);
#endif // _DEBUG	
}

// 前提 色环准备完毕+得到最大两个肤色区域即原本只用来测试的m_cvMaxContour
void FrameProcess::drawClickFinger ()
{	
	// 色环 给m_circleMaskLeft/Right区域上色 已经合并入手型
	// 此处判断手指中心在哪个手型中 就绘制哪个手型
	// 2014-5-21 23:11:26 增加新逻辑 必须左右手同时都找到click
	std::vector<int> leftIdx , rightIdx;
	std::map<float,float>::iterator itBegin = m_fingerTipsLeft.begin ();
	std::map<float,float>::iterator itBeginR = m_fingerTipsRight.begin ();
	//imshow("m_cvMaxContourLeft0",m_cvMaxContourLeft[0]);
	//imshow("m_cvMaxContourLeft1",m_cvMaxContourLeft[1]);
	for(;(itBegin!= m_fingerTipsLeft.end ()); itBegin++){
		for(int idx = 0; idx < 2; idx ++){
			if(((uchar*)(m_cvMaxContourLeft[idx].data + 
				m_cvMaxContourLeft[idx].step.p[0]* (int)itBegin->second))[(int)itBegin->first] > 0){
					m_fingerTipsLeftFinal.insert (std::make_pair (itBegin->first,itBegin->second));
 					if(leftIdx.empty ()
 						|| (!leftIdx.empty () && (leftIdx.back () != idx)))
						leftIdx.push_back(idx);
						break;
			}
		}		
	}
	for(;(itBeginR!= m_fingerTipsRight.end ()); itBeginR++){
		for(int idx = 0; idx < 2; idx ++){
			if(((uchar*)(m_cvMaxContourRight[idx].data + 
				m_cvMaxContourRight[idx].step.p[0]* (int)itBeginR->second))[(int)itBeginR->first] > 0){
					m_fingerTipsRightFinal.insert (std::make_pair (itBeginR->first,itBeginR->second));
 					if(rightIdx.empty ()
 						|| (!rightIdx.empty () && (rightIdx.back () != idx)))
						rightIdx.push_back(idx);
						break;
			}
		}		
	}

	if(m_fingerTipsLeftFinal.size () != m_fingerTipsRightFinal.size ())
		return;
	uchar mask;
	// 手型 给m_cvMaxContour区域上色
	for(int nY = 0; nY < m_cvProcessImgLeft.rows; nY++){
		for(int nX = 0; nX < m_cvProcessImgLeft.cols; nX++){
			if(m_step == 4){
				cv::Vec4b color;
				for(int i = 0; i < leftIdx.size (); i++){
					// left
					color = getColor4(m_cvProcessImgLeft,nX,nY);
					// click手势中的色环也要加入clickleft/right
					mask = ((uchar*)(m_cvMaxContourLeft[leftIdx[i]].data + m_cvMaxContourLeft[leftIdx[i]].step.p[0]*nY))[nX];
					if(mask > 0)
						setColor(m_cvProcessClickLeft,nX,nY,color);
					else
						setColor(m_cvProcessClickLeft,nX,nY,cv::Vec4b(0,0,0,0));
				}
				for(int i = 0; i < rightIdx.size (); i++){
					// right
					color = getColor4(m_cvProcessImgRight,nX,nY);
					mask = ((uchar*)(m_cvMaxContourRight[rightIdx[i]].data + m_cvMaxContourRight[rightIdx[i]].step.p[0]*nY))[nX];
					if(mask > 0)
						setColor(m_cvProcessClickRight,nX,nY,color);
					else
						setColor(m_cvProcessClickRight,nX,nY,cv::Vec4b(0,0,0,0));
				}
			}
			else{
				cv::Vec3b color;
				for(int i = 0; i < leftIdx.size (); i++){
					// left
					color = getColor3(m_cvProcessImgLeft,nX,nY);
					// click手势中的色环也要加入clickleft/right
					mask = ((uchar*)(m_cvMaxContourLeft[leftIdx[i]].data + m_cvMaxContourLeft[leftIdx[i]].step.p[0]*nY))[nX];
					if(mask > 0)
						setColor(m_cvProcessClickLeft,nX,nY,color);
					else
						setColor(m_cvProcessClickLeft,nX,nY,cv::Vec3b(0,0,0));
				}
				for(int i = 0; i < rightIdx.size (); i++){
					// right
					color = getColor3(m_cvProcessImgRight,nX,nY);
					mask = ((uchar*)(m_cvMaxContourRight[rightIdx[i]].data + m_cvMaxContourRight[rightIdx[i]].step.p[0]*nY))[nX];
					if(mask > 0)
						setColor(m_cvProcessClickRight,nX,nY,color);
					else
						setColor(m_cvProcessClickRight,nX,nY,cv::Vec4b(0,0,0,0));
				}
			}
		}
	}
	imshow ("left outputSkinPic",m_cvProcessClickLeft);
	//imwrite ("leftOutputSkinPic.jpg",m_cvProcessClickLeft);
	//imshow ("m_cvProcessClickRight",m_cvProcessClickRight);
	
#ifdef _DEBUG
	//imwrite ("m_cvProcessClickLeft.jpg",m_cvProcessClickLeft);
	//imwrite ("m_cvProcessClickRight.jpg",m_cvProcessClickRight);
#endif // DEBUG
}

void FrameProcess::drawClickFingerRevised (cv::Mat& mask,cv::Mat &processOriginImg,cv::Mat& dst)
{
	// 对原图使用遮罩
	processOriginImg.copyTo (dst,mask);
	//imwrite ("clickFinger.jpg",dst);
}

void FrameProcess::drawClickFingerRevised (std::vector<std::vector<cv::Point> > & contour, int contourIdx,cv::Mat &processOriginImg,cv::Mat& dst)
{
	drawContours (m_cvMaxContourTemp,contours,contourIdx,POWER_2_8-1,CV_FILLED);
	drawClickFingerRevised (m_cvMaxContourTemp,processOriginImg,dst);
}

void FrameProcess::setElement_size (int val)
{
	element_size = val; 
	element = getStructuringElement (cv::MORPH_ELLIPSE, cv::Size (element_size, element_size));
}

void FrameProcess::getOcamModel ()
{
	float sf = 1.3;
	// camera 0
	get_ocam_model (&m_ocam0, "calib_results_0.txt");
	create_perspecive_undistortion_LUT( m_cvUndist32FC1_00, m_cvUndist32FC1_01, &m_ocam0, sf );
	// camera 1
	get_ocam_model (&m_ocam1, "calib_results_1.txt");
	create_perspecive_undistortion_LUT( m_cvUndist32FC1_10, m_cvUndist32FC1_11, &m_ocam1, sf );
}

void FrameProcess::undistortion ()
{	
	// for camera 0	
	remap(m_cvProcessImgLeft,m_cvUndist8UC3_0,m_cvUndist32FC1_00,m_cvUndist32FC1_01,
	//cvRemap( m_cvProcessImgLeft.data, m_cvUndist8UC3_0.data, mapx_persp, mapy_persp,
		CV_INTER_LINEAR,cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0) );
	//imwrite ("distortion0.jpg",m_cvProcessImgLeft);
	//imwrite ("undistortion0.jpg",m_cvUndist8UC3_0);
	// for camera 1	
	remap( m_cvProcessImgRight, m_cvUndist8UC3_1, m_cvUndist32FC1_10,m_cvUndist32FC1_11,
		CV_INTER_LINEAR,cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
	//imwrite ("distortion1.jpg",m_cvProcessImgRight);
	//imwrite ("undistortion1.jpg",m_cvUndist8UC3_1);
}

FrameProcess* FrameProcess::m_pInstance = NULL;
