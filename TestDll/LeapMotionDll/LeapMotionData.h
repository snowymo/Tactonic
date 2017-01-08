#ifndef _LEAP_MOTION_DATA_H
#define _LEAP_MOTION_DATA_H

#include <iostream>
#include "Leap.h"

class LeapMotionData
{
public:
	LeapMotionData(void);
	~LeapMotionData(void);

public:
	Leap::InteractionBox getInteractionBox() const { return m_interactionBox; }
	void setInteractionBox(Leap::InteractionBox val) { m_interactionBox = val; }
	Leap::HandList getHandlist() const { return m_handlist; }
	void setHandlist(Leap::HandList val);
	Leap::FingerList getFingerList() const { return m_fingerList; }
	void setFingerList(Leap::FingerList val) { m_fingerList = val; }
	Leap::GestureList getGestureList() const { return m_gestureList; }
	void setGestureList(Leap::GestureList val) { m_gestureList = val; }
	std::vector<Leap::Vector> getPalmPoses() const { return m_vPalmPoses; }
	void setPalmPoses(std::vector<Leap::Vector> val) { m_vPalmPoses = val; }
	std::vector<Leap::Vector> getPalmNorms() const { return m_vPalmNorms; }
	void setPalmNorms(std::vector<Leap::Vector> val) { m_vPalmNorms = val; }
	std::vector<Leap::FingerList> getFingerListaccdHand() const { return m_vFingerList; }
	void setFingerListaccdHand(std::vector<Leap::FingerList> val) { m_vFingerList = val; }

private:
	Leap::InteractionBox m_interactionBox;
	
	Leap::HandList m_handlist;
	
	Leap::FingerList m_fingerList;
	
	Leap::GestureList m_gestureList;	
	
	// 手们的手心位置 朝向
	std::vector<Leap::Vector> m_vPalmPoses;	
	std::vector<Leap::Vector> m_vPalmNorms;
	// 手们的手指列表
	std::vector<Leap::FingerList> m_vFingerList;
};

#endif