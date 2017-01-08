#ifndef _FRAME_PROCESS_H
#define _FRAME_PROCESS_H

//#include <glm/glm.hpp>
#include "D:\Libs\glm\0.9.3.4\glm\glm.hpp"//你妹妹
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/contrib/contrib.hpp"
#include <vector>
#include "Hand.h"
#include "ocam_functions.h"

#define POWER_2_16 65536
#define POWER_2_8	256
#define SINGLER_FINGER_NUMBER 5
#define HAND_NUMBER 2
#define COS_FINGERTIP 0.67
#define FINGER_PERCENT 0.2

#define IMAGE_NONE	0x00
#define IMAGE_LEFT	0x01
#define IMAGE_RIGHT	0x10
#define IMAGE_BOTH	0x11

enum ACTION
{
	NONE,
	MENU,
	CLICK
};

// 这里尝试一下struct 这个C#里头写一样的就可以了 vector就暂时和ArrayList一致便是
struct ARGlassAction 
{
	ACTION name;	// 就是个枚举类
	ACTION nameOld;	// 就是个枚举类
	cv::Point2f pos;		// vector由三个float组成
	float age;
	ARGlassAction(){
		name = NONE;
		age = -1;
		pos = cv::Point2f(0,0);
	}
};

class HandInfo
{
public:
	HandInfo(){
		fingerPos = new cv::Point[10]();
	}
	~HandInfo(){
		delete [] fingerPos;
		fingerPos = NULL;
	}
public:
	int nfingers;
	cv::Point palmPos;
	cv::Point* fingerPos;
};

class FrameProcess
{
private:
	FrameProcess(void);
	//FrameProcess(cv::Mat mat);
	~FrameProcess(void);

public:
	static FrameProcess* getInstance();

	enum YUV{
		YUV1DOWN,
		YUV1UP,
		YUV2DOWN,
		YUV2UP
	};
	enum { STEREO_BM=0, STEREO_SGBM=1, STEREO_HH=2, STEREO_VAR=3 };

public:
	// 2014-8-25 22:28:29 
	// get ocam model with 2 calib results
	void getOcamModel();
	// undistortion
	void undistortion();

public:
	// getters and setters
	//int Yuv1downside() const { return yuv1downside; }
	int setYuv(YUV yuvPara,int value);
	uchar* getRawImgRight() const { return m_pRawImgRight; }
	void setRawImgRight(uchar* val) { m_pRawImgRight = val; }
	uchar* getRawImgLeft() const { return m_pRawImgLeft; }
	void setRawImgLeft(uchar* val) { m_pRawImgLeft = val; }
	void setRawImg(uchar* val1,uchar* val2);
	void setDetail(int height,int width,int step,bool isPostCalib,bool isStereoRectify);
	cv::Mat getProcessImgRight() const { return m_cvProcessImgRight; }
	void setProcessImgRight(cv::Mat val) { m_cvProcessImgRight = val; }
	cv::Mat getProcessImgLeft() const { return m_cvProcessImgLeft; }
	void setProcessImgLeft(cv::Mat val) { m_cvProcessImgLeft = val; }
	cv::Mat getProcessClickRight() const { return m_cvProcessClickRight; }
	void setProcessClickRight(cv::Mat val) { m_cvProcessClickRight = val; }
	cv::Mat getProcessClickLeft() const { return m_cvProcessClickLeft; }
	void setProcessClickLeft(cv::Mat val) { m_cvProcessClickLeft = val; }

	// blue h:90~150 s:180~255
	cv::Scalar tape_hsv_min;
	cv::Scalar tape_hsv_max;
	
	int getElement_size() const { return element_size; }
	void setElement_size(int val);
public:
	void processSkinImg(int type);
	void getAction(int type, ARGlassAction* menuActions, int* menuActionCnt, ARGlassAction* clickActions, int* clickActionCnt);
	// 校正双目图像与各自内参 得到新的图像
	void stereoMatch();
	// 视差处理
	void processDisparity();
	// 指环提取
	void processFingerCircle();

	void addUniquenessRatio(double val) { m_uniquenessRatio += val; }
	double getUniquenessRatio() const { return m_uniquenessRatio; }
	
private:
	// transfer images according to the extrinct 逻辑上来看 应该是把左图根据外参进行RT T的水平向量先无视
	void calibLeftImage();
	void transferCoord(double & x,double & y, int centerx, int centery);
	void findContour( cv::Mat src,std::vector<std::vector<cv::Point> > &contours , cv::Mat& maxContour,cv::Mat &dst ,cv::Mat & processOriginImg,cv::Mat& processClickImg);
	void findContour( cv::Mat src,std::vector<std::vector<cv::Point> > &contours , cv::Mat* maxContour,cv::Mat &dst);
	void getIntrExtrinsicInput();

	void stereoMatch(cv::Mat& cvLeft,cv::Mat& cvRight);
	void processDisparity(cv::Mat& cvLeft, cv::Mat& cvRight, cv::string name);


	cv::Vec4b getColor4( cv::Mat img,int nX, int nY );
	cv::Vec3b getColor3( cv::Mat img,int nX, int nY );
	cv::Vec4b getColor4(uchar * img,int nX, int nY);
	cv::Vec3b getColor3(uchar * img,int nX, int nY);
	cv::Vec4b getColor4(uchar * img,double nX, double nY);
	cv::Vec3b getColor3(uchar * img,double nX, double nY);
	bool isValidXY(int target,int maxSize);

	void setColor( cv::Mat &img,int nX, int nY, cv::Vec4b );
	void setColor( cv::Mat &img,int nX, int nY, cv::Vec3b );
	bool isSkin( cv::Vec4b color );
	bool isSkin( cv::Vec3b color );
	void getFingers( cv::Mat &img, cv::Mat * maxContour, HandInfo * hands);
	void getFingers( cv::Mat &img, cv::Mat & maxContour, HandInfo * hands ,cv::Mat &processOriginImg,cv::Mat & processClickImg);
	int getMaxContour( std::vector<std::vector<cv::Point> > cvHandContourPoints);
	int approxFinger( std::vector<cv::Point > cvHandContourPoints, cv::Mat &cvFingers, cv::Point &palmPos, cv::Point* fingerPos );
	// 制作click手型图 给手型+色环上色
	void drawClickFinger();
	// 制作click手型图 给含有色环的手型上色
	void drawClickFingerRevised(cv::Mat& mask,cv::Mat &processOriginImg,cv::Mat& dst);
	void drawClickFingerRevised (std::vector<std::vector<cv::Point> > & contour, int contourIdx,cv::Mat &processOriginImg,cv::Mat& dst);

private:
	double angle( cv::Point p1, cv::Point p2, cv::Point p3 );
	long double dis(cv::Point p1,cv::Point p2);
	void createCalibrationMatrix();
	void createStereoPara();

private:
	static FrameProcess* m_pInstance;

	// left image and right image
	uchar* m_pRawImgLeft,* m_pRawImgRight;
	cv::Mat m_cvProcessImgLeft,m_cvProcessImgRight;
	cv::Mat m_cvProcessClickLeft,m_cvProcessClickRight;
	
	// left skin image and right skin image
	cv::Mat m_cvSkinImgLeft,m_cvSkinImgRight;
	bool m_bInitSkin;
	cv::Mat m_hsvImgLeft,m_hsvImgRight;
	cv::Mat m_circleMaskLeft,m_circleMaskRight,m_circleMaskHelp;
	HandInfo * m_handInfos;
	std::map<float,float> m_fingerTipsLeft,m_fingerTipsRight;
	std::map<float,float> m_fingerTipsLeftFinal,m_fingerTipsRightFinal;

	// details
	int m_step,m_height,m_width;
	bool m_bPostCalib;	// 如果为真 则unity后期做基于标定的旋转
	bool m_bStereoRectify;	// 如果为真 则进行立体校正
	// adjust yuv
	int m_yuv1downside,m_yuv1upside,m_yuv2downside,m_yuv2upside;	
	// stereo para
	int m_uniquenessRatio;
	// element size
	int element_size;
	

	//Hand * m_leftHand;
	// calibration matrix
	glm::mat3x3 m_rotateMatrix;
	cv::Mat m_cvExRotateMatrix;
	glm::vec3 m_transMatrix;
	cv::Mat m_cvExTransMatrix;
	glm::mat3x3 m_intrinsics;
	cv::Mat m_cvInM1,m_cvInM2,m_cvInD1,m_cvInD2;
	cv::Mat m_cvInM1Inv,m_cvExRotateMatrixInv;
	cv::Mat m_cvR1,m_cvR2,m_cvP1,m_cvP2,m_cvQ;
	cv::Rect m_rectRoi1,m_rectRoi2;
	int m_alg,m_numberOfDisparities;
	cv::Mat m_cvMaxContourLeft[2],m_cvMaxContourRight[2];//8UC1
	cv::Mat m_cvMaxContourTemp;//8UC1
	
	// for function use
	cv::Mat m_cvTemp;//8UC1
	cv::Mat m_cvContourDst;//8UC1
	cv::Mat m_cvFingers;// the same as processImg
	std::vector<cv::Vec4i> hierarchy;
	int drawLevel;
	double epsilon;
	bool isClosed,isFound;
	int edge;
	std::vector<cv::Point> approx;
	std::vector<std::vector<cv::Point>> dst,handdst;
	std::vector<std::vector<cv::Point> > cvHandContourPoints;
	cv::FileStorage fs;
	cv::Mat map11, map12, map21, map22;
	cv::Mat img1r, img2r;
	cv::StereoBM bm;
	cv::StereoSGBM sgbm;
	cv::StereoVar var;
	cv::Mat disp, disp8;
	cv::Mat m_cvApprox;
	cv::Mat element;
	std::vector< std::vector< cv::Point > > contours;
	cv::Point2f center;
	float radius;
	int maxContourIdx,maxContourIdx2;
	int clickContourIdx;

public:	
	// 2014-8-25 22:28:44
	// ocam model
	struct ocam_model m_ocam0,m_ocam1;
	cv::Mat m_cvUndist8UC3_0, m_cvUndist8UC3_1;
	cv::Mat m_cvUndist32FC1_00, m_cvUndist32FC1_01,m_cvUndist32FC1_10, m_cvUndist32FC1_11;
	CvMat* mapy_persp, *mapx_persp;
};


#endif