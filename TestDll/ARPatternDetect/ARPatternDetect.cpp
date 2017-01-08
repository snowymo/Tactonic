#include "ARPatternDetect.h"
#include <windows.h>
//#include <opencv2/opencv.hpp>

ARPatternDetect::ARPatternDetect ()
{
	gl_para = new double[16];
	memset (gl_para,0,sizeof(double)*16);

	m_bPatternReady = true;
	double center[2];
	center[0] = center[1] = 0;
	
	// hiro pattern
	//patt_name = "D:\Projects\ARGlass\branches\Unity\vst\Data\patt.hiro";
	// 用三个pattern初始化
	m_vPattern.push_back (ARTPattern("Data\\patt.hiro","",0,center,80.0));
	m_vPattName.push_back ("Data\\patt.hiro");
	m_vConfigName.push_back ("");
	m_vPattern.push_back (ARTPattern("Data\\patt.sample1","",0,center,80.0));
	m_vPattName.push_back ("Data\\patt.sample1");
	m_vConfigName.push_back ("");
	m_vPattern.push_back (ARTPattern("","Data/multi/marker.dat",0,center,40.0));
	m_vPattName.push_back ("");
	m_vConfigName.push_back ("Data/multi/marker.dat");
	m_vConfig.push_back (new ARMultiMarkerInfoT());
	m_vConfig.push_back (new ARMultiMarkerInfoT());
	m_vConfig.push_back (new ARMultiMarkerInfoT());

	// multi marker config
	for(int i = 0; i < m_vPattern.size (); i++){
		if(m_vPattName[i] != ""){
			if( (m_vPattern[i].m_patt_id = arLoadPatt (m_vPattName[i].c_str ()) ) < 0){
				printf("pattern data load error !!\n");
			}
		}
		if(m_vConfigName[i] != "")
			if((m_vConfig[i] = arMultiReadConfigFile (m_vConfigName[i].c_str ()) ) == NULL ){
				printf("config data load error !!\n");
			}
	}

	thresh = 100;
}


ARPatternDetect::~ARPatternDetect(void)
{
	if(gl_para)
		delete [] gl_para;
	gl_para = NULL;
}

bool ARPatternDetect::detectPattern (unsigned char * image)
{
	if(!m_bPatternReady)
		return false;

	// init
	bool isDetect = false;
	marker_num = 0;
	marker_info = NULL;
	for(int i = 0; i < m_vPattern.size (); i++)
		m_vPattern[i].m_visible = -1;

	/* detect the multi markers in the video frame */
	// output input file
// 	cv::Mat leftImg = cv::Mat(480,640,CV_8UC4,image);
// 	imwrite ("leftImg.jpg",leftImg);
// 	cvtColor (leftImg,leftImg,CV_RGBA2BGRA);
// 	imwrite ("leftImg2.jpg",leftImg);
	if( arDetectMarkerLite(image/*leftImg.data*/, thresh, &marker_info, &marker_num) < 0 ) {
		return false;
	}
	/* detect the hiro markers in the video frame */
// 	if( arDetectMarker(leftImg.data, thresh, &marker_info2, &marker_num2) < 0 ) {
// 		return false;
// 	}
	int j, k;
	// 将multi部分的结果加入vector
	for(int i = 0; i < m_vPattern.size (); i++){
		if(m_vPattName[i] != ""){
			/* check for object visibility */
			k = -1;
			for( j = 0; j < marker_num; j++ ) {
				if( m_vPattern[i].m_patt_id == marker_info[j].id ) {
					if( k == -1 ) 
						k = j;
					else if( marker_info[k].cf < marker_info[j].cf )
						k = j;
					m_vPattern[i].m_visible = k;
					/* get the transformation between the marker and the real camera */
					//double center[2] = {m_vPattern[i].m_center.x,m_vPattern[i].m_center.y};
					arGetTransMat(&marker_info[k], m_vPattern[i].m_center, m_vPattern[i].m_width, m_vPattern[i].m_trans);
					// Get the translation and the rotation 
					arUtilMat2QuatPos (m_vPattern[i].m_trans,m_vPattern[i].m_rotation,m_vPattern[i].m_translation);
				}
			}
			if(k != -1)
				isDetect = true;
		}
		else if(m_vConfigName[i]!= ""){
			double err = 0;
			if( (err=arMultiGetTransMat(marker_info, marker_num, m_vConfig[i])) < 0 ) {
				break;
			}
			else if(err < 100)
				isDetect = true;
			printf("err = %f\n", err);
			for(int j = 0; j < m_vConfig[i]->marker_num; j++){
				// 根据multi配置里的情况得知要绘制几个方块块
				if(m_vConfig[i]->marker[j].visible >= 0){
					// Get the translation and the rotation 
					arUtilMat2QuatPos (m_vConfig[i]->trans,m_vPattern[i].m_rotation,m_vPattern[i].m_translation);
					++m_vPattern[i].m_visible;
					//arUtilMat2QuatPos (m_vPattern[i].m_config->marker[i].trans,m_rotation,m_translation);
				}
			}
		}
	}
	
	/* load the camera transformation matrix */
	/*argConvGlpara(patt_trans, gl_para);*/

	// turn to glm mat
// 	for(int i = 0; i < 16; i++)
// 		glm_para[i/4][i%4] = gl_para[i];
// 	// turn to glm point
// 	glm_point[0] = glm_para * glm::vec4(10,0,0,1);
// 	glm_point[1] = glm_para * glm::vec4(10,10,0,1);
// 	glm_point[2] = glm_para * glm::vec4(10,0,10,1);
// 	glm_point[3] = glm_para * glm::vec4(10,10,10,1);
// 	glm_point[4] = glm_para * glm::vec4(0,10,0,1);
// 	glm_point[5] = glm_para * glm::vec4(0,10,10,1);
// 	glm_point[6] = glm_para * glm::vec4(0,0,10,1);
// 	glm_point[7] = glm_para * glm::vec4(0,0,0,1);

	return isDetect;
}

void ARPatternDetect::init ()
{
	/* open the video path */
	char			*vconf = "Data\\WDM_camera_flipV.xml";
	if( arVideoOpen( vconf ) < 0 ) exit(0);
	/* find the size of the window */
	if( arVideoInqSize(&xsize, &ysize) < 0 ) exit(0);
	printf("Image size (x,y) = (%d,%d)\n", xsize, ysize);

	/* set the initial camera parameters */
	char *cparam_name = "Data/camera_para.dat";
	if( arParamLoad(cparam_name, 1, &wparam) < 0 ) {
		printf("Camera parameter load error !!\n");
		exit(0);
	}
	
	arParamChangeSize( &wparam, xsize, ysize, &cparam );
	arInitCparam( &cparam );
	printf("*** Camera Parameter ***\n");
	arParamDisp( &cparam );
}

// ARPatternDetect* ARPatternDetect::getInstance (bool isInitial)
// {
// 	m_bInit = isInitial;
// 	if(!m_pInst)
// 		m_pInst = new ARPatternDetect;
// 	return m_pInst;
// }

ARPatternDetect* ARPatternDetect::getInstance ()
{
	if(!m_pInst)
		m_pInst = new ARPatternDetect;
	return m_pInst;
}

void ARPatternDetect::cleanup ()
{
	//arVideoCapStop();
/*	if(!m_bInit){*/
		arVideoClose();
// 		m_bInit = true;
// 	}
	//argCleanup();
}

void ARPatternDetect::getGlm_point (float * fl_point) const
{
	for(int i = 0; i < 4 * 8; i++)
		fl_point[i] = glm_point[i/4][i%4];
}

void ARPatternDetect::getResult (ARTCSPattern * results)
{
	for(int i = 0; i < m_vPattern.size (); i++){
		results[i].m_visible = m_vPattern[i].m_visible;
		if(results[i].m_visible != -1){
			results[i].m_patt_id = m_vPattern[i].m_patt_id;
			results[i].m_model_id = m_vPattern[i].m_model_id;		
			results[i].m_width = m_vPattern[i].m_width;
			//memcpy_s (results[i].m_center,sizeof(double) * 2,m_vPattern[i].m_center,sizeof(double) * 2);
			results[i].m_center.x = m_vPattern[i].m_center[0];
			results[i].m_center.y = m_vPattern[i].m_center[1];

			//memcpy_s (results[i].m_trans,sizeof(double) * 12,m_vPattern[i].m_trans,sizeof(double) * 12);
			memcpy_s (&results[i].m_rotation,sizeof(double) * 4,m_vPattern[i].m_rotation,sizeof(double) * 4);
			memcpy_s (&results[i].m_translation,sizeof(double) * 3,m_vPattern[i].m_translation,sizeof(double) * 3);
			// 		memcpy_s (results[i].m_config_name,sizeof(m_vPattern[i].m_config_name),m_vPattern[i].m_config_name,sizeof(m_vPattern[i].m_config_name));
			// 		memcpy_s (results[i].m_patt_name,sizeof(m_vPattern[i].m_patt_name),m_vPattern[i].m_patt_name,sizeof(m_vPattern[i].m_patt_name));
		}
	}
}

ARPatternDetect * ARPatternDetect::m_pInst = NULL;
