#include "LeapMotionData.h"


LeapMotionData::LeapMotionData(void)
{
	// 以防万一 多push几个
	for(int i = 0; i < 5; i++){
		m_vPalmPoses.push_back (Leap::Vector(0,0,0));
		m_vPalmNorms.push_back (Leap::Vector(0,0,0));
		m_vFingerList.push_back (Leap::FingerList());
	}
}


LeapMotionData::~LeapMotionData(void)
{
}

void LeapMotionData::setHandlist (Leap::HandList val)
{
	m_handlist = val;
	for(int i = 0; i < m_handlist.count (); i++){
		m_vPalmPoses[i] = m_handlist[i].palmPosition();
		m_vPalmNorms[i] = m_handlist[i].palmNormal();
		m_vFingerList[i] = m_handlist[i].fingers();	// 每个obj都有自己的id的 想想怎么用 最起码 点击菜单的手势是可以做到的 
	}
}
