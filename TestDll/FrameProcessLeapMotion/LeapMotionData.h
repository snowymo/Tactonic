#ifndef _LEAP_MOTION_DATA_H
#define _LEAP_MOTION_DATA_H

#include <iostream>
#include "Leap.h"

// 把骨头的情况传递给西夏普
struct LeapBone
{
	Leap::Vector pos;
	float width;
	float length;
	LeapBone(){
		pos = Leap::Vector();
		width = 0;
		length = 0;
	}
};

// TYPE_METACARPAL = 0,           /**< Bone connected to the wrist inside the palm */
// 	TYPE_PROXIMAL = 1,     /**< Bone connecting to the palm */
// 	TYPE_INTERMEDIATE = 2, /**< Bone between the tip and the base*/
// 	TYPE_DISTAL = 3,       /**< Bone at the tip of the finger */
struct LeapFinger
{
	LeapBone metacarpalBone;
	LeapBone ProximalBone;
	LeapBone intermediateBone;
	LeapBone distalBone;
	LeapFinger(){
		metacarpalBone = distalBone = intermediateBone = ProximalBone = LeapBone();
	}
};

// TYPE_THUMB  = 0, /**< The thumb */
// 	TYPE_INDEX  = 1, /**< The index or fore-finger */
// 	TYPE_MIDDLE = 2, /**< The middle finger */
// 	TYPE_RING   = 3, /**< The ring finger */
// 	TYPE_PINKY  = 4  /**< The pinky or little finger */
struct LeapHand
{
	LeapFinger thumbFinger;
	LeapFinger indexFinger;
	LeapFinger middleFinger;
	LeapFinger ringFinger;
	LeapFinger pinkyFinger;
	LeapHand(){
		thumbFinger = indexFinger = middleFinger = ringFinger = pinkyFinger = LeapFinger();
	}
};

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
	int64_t getFrameid() const { return m_frameid; }
	void setFrameid(int64_t val) { m_frameid = val; }
	void getBoneInfos(LeapHand * leapHandBones) const;
	void setBoneInfos(Leap::HandList val) ;
	
private:
	void setBoneInfos(LeapBone& bone,Leap::Bone constBone) ;
	void initialBones(int handIdx);

private:
	Leap::InteractionBox m_interactionBox;
	
	Leap::HandList m_handlist;
	
	Leap::FingerList m_fingerList;
	
	Leap::GestureList m_gestureList;	

	int64_t m_frameid;

	// 手们的手心位置 朝向
	std::vector<Leap::Vector> m_vPalmPoses;	
	std::vector<Leap::Vector> m_vPalmNorms;
	// 手们的手指列表
	std::vector<Leap::FingerList> m_vFingerList;

	LeapHand m_boneInfos[2];
	
};

#endif