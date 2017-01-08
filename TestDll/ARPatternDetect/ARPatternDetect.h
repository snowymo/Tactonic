#ifndef _AR_PATTERN_DETECT_H
#define _AR_PATTERN_DETECT_H

#include "ARPattern.h"
#include <string.h>
#include <AR/gsub.h>
#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <glm/glm.hpp>
#include <vector>

// 对输入的图片进行pattern识别

class ARPatternDetect
{
private:
	ARPatternDetect();
	~ARPatternDetect(void);

public:
	/*static ARPatternDetect* getInstance(bool isInitial);*/
	static ARPatternDetect* getInstance();

public:
	void init();
	bool detectPattern(unsigned char * image);
	void cleanup();
	
public:
	double* getGl_para() const { return gl_para; }
	void setGl_para(double* val) { gl_para = val; }
	void getGlm_point(float * fl_point) const; 
	//void setGlm_point(glm::vec4 val) { glm_point = val; }
	void getResult(ARTCSPattern * results );
// 	void getRotation(double * val) const { memcpy_s (val,sizeof(double) * 4,m_rotation,sizeof(double)*4);}
// 	void setRotation(double * val) { m_rotation = val; }
// 	void getTranslation(double * val) const { memcpy_s (val,sizeof(double) * 3,m_translation,sizeof(double)*3);}
// 	void setTranslation(double * val) { m_translation = val; }

private:
	double* gl_para;
	glm::mat4x4 glm_para;
	glm::vec4 glm_point[8];
	
	std::vector<ARTPattern> m_vPattern;// 进化版本 有多个pattern	
	std::vector<ARMultiMarkerInfoT*> m_vConfig;
	std::vector<std::string> m_vPattName,m_vConfigName;

	bool m_bPatternReady;
	ARUint8 *dataPtr;
	ARMarkerInfo *marker_info,*marker_info2;
	int thresh;
	int marker_num,marker_num2;
	
	int             xsize, ysize;
	ARParam wparam;
	ARParam cparam;

	static ARPatternDetect * m_pInst;
};

#endif