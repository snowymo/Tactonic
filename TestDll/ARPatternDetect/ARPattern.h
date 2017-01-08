#ifndef _AR_PATTERN_H
#define _AR_PATTERN_H

#include <AR/arMulti.h>
#include <string>

struct Point2D
{
	double x;
	double y;
};

struct Point3D
{
	double x;
	double y;
	double z;
};

struct Point4D
{

	double x;
	double y;
	double z;
	double w;
};

struct ARTCSPattern
{
	int m_patt_id;
	int m_model_id;
	int m_visible;
	double m_width;
	Point2D m_center;
	Point4D m_rotation;	
	Point3D m_translation;
};

class ARTPattern
{
public:

	std::string m_patt_name;
	std::string m_config_name;
	int m_patt_id;
	int m_model_id;
	int m_visible;
	double m_width;
	double m_center[2];	
	double m_trans[3][4];
	double m_rotation[4];	
	double m_translation[3];

public: 
	ARTPattern(){
		m_visible = -1;
	}

	ARTPattern(char* patt_name,char * config_name,int model_id,double * center,double width){
		m_patt_name = std::string(patt_name);
		m_config_name = std::string(config_name);
		m_model_id = model_id;
		memcpy_s (m_center,sizeof(double) * 2,center,sizeof(double) * 2);
		
		m_width = width;
		m_visible = -1;
		m_patt_id = -1;
		m_model_id = -1;
		memset (m_trans,0,sizeof(double) * 3 * 4);
		memset (m_rotation,0,sizeof(double) * 4);
		memset (m_translation,0,sizeof(double) * 3);
	}
	~ARTPattern(){}
};

#endif