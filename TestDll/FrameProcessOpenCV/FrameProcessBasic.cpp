// 2014-3-26 1:42:13
// author:hehe
// 本cpp为FrameProcess类的基本函数实现

#include "FrameProcess.h"

cv::Vec4b FrameProcess::getColor4 (cv::Mat img,int nX, int nY)
{
	if(isValidXY (nX,m_width) && isValidXY (nY,m_height))
		return ((cv::Vec4b*)(img.data + img.step.p[0]*nY))[nX];
	else
		return cv::Vec4b(0,0,0,255);
}

cv::Vec4b FrameProcess::getColor4 (uchar * img,int nX, int nY)
{
	if(isValidXY (nX,m_width) && isValidXY (nY,m_height))
		return ((cv::Vec4b*)img)[nX + nY * m_width];
	else
		return cv::Vec4b(0,0,0,255);
	//return ((cv::Vec4b*)(img[nX * 4 + nY * m_width * 4]))[0];
}

cv::Vec4b FrameProcess::getColor4 (uchar * img,double nX, double nY)
{
	double leftPadding = nX - (int)nX;
	double topPadding = nY - (int)nY;
	cv::Vec4b color = cv::Vec4b(getColor4 (img,(int)nX,(int)nY) * (1 - leftPadding) * (1 - topPadding)
		+ getColor4 (img,(int)nX + 1,(int)nY) * leftPadding * (1 - topPadding)
		+ getColor4 (img,(int)nX,(int)nY + 1) * (1 - leftPadding) * topPadding
		+ getColor4 (img,(int)nX + 1,(int)nY + 1) * leftPadding * topPadding);
	return color;
}

cv::Vec3b FrameProcess::getColor3 (cv::Mat img,int nX, int nY)
{
	if(isValidXY (nX,m_width) && isValidXY (nY,m_height))
		return ((cv::Vec3b*)(img.data + img.step.p[0]*nY))[nX];
	else
		return cv::Vec3b(0,0,0);
}

cv::Vec3b FrameProcess::getColor3 (uchar * img,int nX, int nY)
{
	if(isValidXY (nX,m_width) && isValidXY (nY,m_height))
		return ((cv::Vec3b*)img)[nX + nY * m_width];
	else
		return cv::Vec3b(0,0,0);
}

cv::Vec3b FrameProcess::getColor3 (uchar * img,double nX, double nY)
{
	double leftPadding = nX - (int)nX;
	double topPadding = nY - (int)nY;
	cv::Vec3b color = cv::Vec3b(getColor3 (img,(int)nX,(int)nY) * (1 - leftPadding) * (1 - topPadding)
		+ getColor3 (img,(int)nX + 1,(int)nY) * leftPadding * (1 - topPadding)
		+ getColor3 (img,(int)nX,(int)nY + 1) * (1 - leftPadding) * topPadding
		+ getColor3 (img,(int)nX + 1,(int)nY + 1) * leftPadding * topPadding);
	return color;
}

void FrameProcess::setColor (cv::Mat &img,int nX, int nY, cv::Vec4b color)
{
	if(isValidXY(nX,m_width) && isValidXY (nY,m_height))
		((cv::Vec4b*)(img.data + img.step.p[0]*nY))[nX] = color;
}

void FrameProcess::setColor (cv::Mat &img,int nX, int nY, cv::Vec3b color)
{
	if(isValidXY(nX,m_width) && isValidXY (nY,m_height))
		((cv::Vec3b*)(img.data + img.step.p[0]*nY))[nX] = color;
}

double FrameProcess::angle (cv::Point pt0, cv::Point pt1, cv::Point pt2)
{
	double dx1 = pt0.x - pt1.x;
	double dy1 = pt0.y - pt1.y;
	double dx2 = pt2.x - pt1.x;
	double dy2 = pt2.y - pt1.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

long double FrameProcess::dis (cv::Point p1,cv::Point p2)
{
	return (p1.x-p2.x)*(p1.x-p2.x)
		+(p1.y-p2.y)*(p1.y-p2.y);
}

void FrameProcess::transferCoord (double & x,double & y, int centerx, int centery)
{
	// R * (x,y,1) + T(0,Ty,Tz) = (x',y',z)
	//glm::vec3 ori(x-centerx,y-centery,1);
	//cv::Mat3d cvOri = cv::Mat3d::zeros (1,3);
	//cv::Mat3d cvCenter = cv::Mat3d::zeros (1,3);
	cv::Mat cvOri = cv::Mat::zeros (3,1,CV_64FC1);
	cv::Mat cvCenter = cv::Mat::zeros (3,1,CV_64FC1);
	
	((double*)cvCenter.data)[0] = centerx;
	((double*)cvCenter.data)[1] = centery;
	((double*)cvCenter.data)[2] = 0;
	
	((double*)cvOri.data)[0] = x;
	((double*)cvOri.data)[1] = y;
	((double*)cvOri.data)[2] = 1;

	// P ^ -1 * ori
	//ori = glm::inverse(m_intrinsics) * ori;
	cvOri = m_cvInM1Inv * (cvOri-cvCenter);
	//ori -= m_transMatrix;
	//ori = ori * glm::inverse (m_rotateMatrix);//m_rotateMatrix._inverse();		
	//ori = ori * m_rotateMatrix;
	//ori = glm::inverse (m_rotateMatrix) * ori;
	//ori =  glm::inverse(m_rotateMatrix) * ori;
	cvOri = m_cvExRotateMatrixInv * cvOri;
	// P * ori
	//ori = m_intrinsics * ori;
	cvOri = m_cvInM1Inv * cvOri;
	// → (x'/z,y'/z,1)
	//ori /= ori.z;
	cvOri = cvOri / ((double*)cvOri.data)[2];
	//ori += glm::vec3(centerx,centery,0);	
	cvOri = cvOri + cvCenter;
	//x = ori.x;
	x = ((double*)cvOri.data)[0];
	//y = ori.y;
	y = ((double*)cvOri.data)[1];
}

bool FrameProcess::isValidXY (int target,int maxSize)
{
	if(target < 0 )
		return false;
	if(target >= maxSize)
		return false;
	return true;
}

bool FrameProcess::isSkin (cv::Vec4b color)
{
	// 根据实验数据来得出具体的肤色辨识值
	float r = color[0];
	float g = color[1];
	float b = color[2];
	float a = color[3];

	unsigned int yuvColor[3] = 
	{
		(unsigned int)(0.299 * r + 0.587 * g+ 0.114 * b),
		(unsigned int)(-0.147 * r - 0.289 * g + 0.436 * b) + 128,
		(unsigned int)(0.615 * r - 0.515 * g - 0.100 * b + 128)
	};
	if (yuvColor[1] >= m_yuv1downside && yuvColor[1] <= m_yuv1upside &&
		yuvColor[2] >= m_yuv2downside && yuvColor[2] <= m_yuv2upside)
		return true;
	else
		return false;
}

bool FrameProcess::isSkin (cv::Vec3b color)
{
	// 根据实验数据来得出具体的肤色辨识值
	float r = color[2];
	float g = color[1];
	float b = color[0];

	unsigned int yuvColor[3] = 
	{
		(unsigned int)(0.299 * r + 0.587 * g+ 0.114 * b),
		(unsigned int)(-0.147 * r - 0.289 * g + 0.436 * b) + 128,
		(unsigned int)(0.615 * r - 0.515 * g - 0.100 * b + 128)
	};
	if (yuvColor[1] >= m_yuv1downside && yuvColor[1] <= m_yuv1upside &&
		yuvColor[2] >= m_yuv2downside && yuvColor[2] <= m_yuv2upside)
		return true;
	else
		return false;
}