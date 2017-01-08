#include "LeapMotionData.h"


LeapMotionData::LeapMotionData(void)
{
	// 以防万一 多push几个
	for(int i = 0; i < 10; i++){
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

void LeapMotionData::setBoneInfos (Leap::HandList val)
{
	// 对传递进来的手型列表进行骨骼信息设值
	int handIdx = 0;
	if((val.count () > 2) || (val.count () == 0)){
		initialBones (0);
		initialBones (1);
		return ;
	}
	else if(val.count () == 1){
		if(val[0].isLeft())
			initialBones (1);
		else
			initialBones (0);
	}
	for(int i = 0; i < val.count (); i++){
		if(val[i].isLeft())
			handIdx = 0;
		else
			handIdx = 1;
		Leap::FingerList fl = val[i].fingers();
		// index finger	// metacarpal bone
		setBoneInfos(m_boneInfos[handIdx].indexFinger.metacarpalBone,fl[Leap::Finger::TYPE_INDEX].bone (Leap::Bone::TYPE_METACARPAL));
	
		// index finger	// proximal bone
		setBoneInfos(m_boneInfos[handIdx].indexFinger.ProximalBone,fl[Leap::Finger::TYPE_INDEX].bone (Leap::Bone::TYPE_PROXIMAL));
		
		// index finger	// intermediate bone
		setBoneInfos(m_boneInfos[handIdx].indexFinger.intermediateBone,fl[Leap::Finger::TYPE_INDEX].bone (Leap::Bone::TYPE_INTERMEDIATE));
		
		// index finger	// distal bone
		setBoneInfos(m_boneInfos[handIdx].indexFinger.distalBone,fl[Leap::Finger::TYPE_INDEX].bone (Leap::Bone::TYPE_DISTAL));


		// thumb finger	// metacarpal bone
		setBoneInfos(m_boneInfos[handIdx].thumbFinger.metacarpalBone,fl[Leap::Finger::TYPE_THUMB].bone (Leap::Bone::TYPE_METACARPAL));
		
		// thumb finger	// proximal bone
		setBoneInfos(m_boneInfos[handIdx].thumbFinger.ProximalBone,fl[Leap::Finger::TYPE_THUMB].bone (Leap::Bone::TYPE_PROXIMAL));
		
		// thumb finger	// intermediate bone
		setBoneInfos(m_boneInfos[handIdx].thumbFinger.intermediateBone,fl[Leap::Finger::TYPE_THUMB].bone (Leap::Bone::TYPE_INTERMEDIATE));
		
		// thumb finger	// distal bone
		setBoneInfos(m_boneInfos[handIdx].thumbFinger.distalBone,fl[Leap::Finger::TYPE_THUMB].bone (Leap::Bone::TYPE_DISTAL));
		

		// middle finger	// metacarpal bone
		setBoneInfos(m_boneInfos[handIdx].middleFinger.metacarpalBone,fl[Leap::Finger::TYPE_MIDDLE].bone (Leap::Bone::TYPE_METACARPAL));
		
		// middle finger	// proximal bone
		setBoneInfos(m_boneInfos[handIdx].middleFinger.ProximalBone,fl[Leap::Finger::TYPE_MIDDLE].bone (Leap::Bone::TYPE_PROXIMAL));
		
		// middle finger	// intermediate bone
		setBoneInfos(m_boneInfos[handIdx].middleFinger.intermediateBone,fl[Leap::Finger::TYPE_MIDDLE].bone (Leap::Bone::TYPE_INTERMEDIATE));
		
		// middle finger	// distal bone
		setBoneInfos(m_boneInfos[handIdx].middleFinger.distalBone,fl[Leap::Finger::TYPE_MIDDLE].bone (Leap::Bone::TYPE_DISTAL));


		// pinky finger	// metacarpal bone
		setBoneInfos(m_boneInfos[handIdx].pinkyFinger.metacarpalBone,fl[Leap::Finger::TYPE_PINKY].bone (Leap::Bone::TYPE_METACARPAL));
		
		// pinky finger	// proximal bone
		setBoneInfos(m_boneInfos[handIdx].pinkyFinger.ProximalBone,fl[Leap::Finger::TYPE_PINKY].bone (Leap::Bone::TYPE_PROXIMAL));
		
		// pinky finger	// intermediate bone
		setBoneInfos(m_boneInfos[handIdx].pinkyFinger.intermediateBone,fl[Leap::Finger::TYPE_PINKY].bone (Leap::Bone::TYPE_INTERMEDIATE));
		
		// pinky finger	// distal bone
		setBoneInfos(m_boneInfos[handIdx].pinkyFinger.distalBone,fl[Leap::Finger::TYPE_PINKY].bone (Leap::Bone::TYPE_DISTAL));
		

		// ring finger	// metacarpal bone
		setBoneInfos(m_boneInfos[handIdx].ringFinger.metacarpalBone,fl[Leap::Finger::TYPE_RING].bone (Leap::Bone::TYPE_METACARPAL));
		
		// ring finger	// proximal bone
		setBoneInfos(m_boneInfos[handIdx].ringFinger.ProximalBone,fl[Leap::Finger::TYPE_RING].bone (Leap::Bone::TYPE_PROXIMAL));
		
		// ring finger	// intermediate bone
		setBoneInfos(m_boneInfos[handIdx].ringFinger.intermediateBone,fl[Leap::Finger::TYPE_RING].bone (Leap::Bone::TYPE_INTERMEDIATE));
		
		// ring finger	// distal bone
		setBoneInfos(m_boneInfos[handIdx].ringFinger.distalBone,fl[Leap::Finger::TYPE_RING].bone (Leap::Bone::TYPE_DISTAL));
		
	}
}

void LeapMotionData::setBoneInfos (LeapBone& bone,Leap::Bone constBone)
{
	if(constBone.isValid ()){
		bone.pos = m_interactionBox.normalizePoint (constBone.nextJoint ()) *40;
		bone.width = constBone.width ();
		bone.length = constBone.length ();
	}
	else
		bone.length = 0;
	//2014-6-16 19:20:43 
}

void LeapMotionData::getBoneInfos (LeapHand * leapHandBones) const
{
	for(int i = 0; i < 2; i++){
		leapHandBones[i] = m_boneInfos[i];
	}
}

void LeapMotionData::initialBones (int handIdx)
{
	setBoneInfos(m_boneInfos[handIdx].indexFinger.metacarpalBone,Leap::Bone::invalid ());
	// index finger	// metacarpal bone
	setBoneInfos(m_boneInfos[handIdx].indexFinger.metacarpalBone,Leap::Bone::invalid ());

	// index finger	// proximal bone
	setBoneInfos(m_boneInfos[handIdx].indexFinger.ProximalBone,Leap::Bone::invalid ());

	// index finger	// intermediate bone
	setBoneInfos(m_boneInfos[handIdx].indexFinger.intermediateBone,Leap::Bone::invalid ());

	// index finger	// distal bone
	setBoneInfos(m_boneInfos[handIdx].indexFinger.distalBone,Leap::Bone::invalid ());


	// thumb finger	// metacarpal bone
	setBoneInfos(m_boneInfos[handIdx].thumbFinger.metacarpalBone,Leap::Bone::invalid ());

	// thumb finger	// proximal bone
	setBoneInfos(m_boneInfos[handIdx].thumbFinger.ProximalBone,Leap::Bone::invalid ());

	// thumb finger	// intermediate bone
	setBoneInfos(m_boneInfos[handIdx].thumbFinger.intermediateBone,Leap::Bone::invalid ());

	// thumb finger	// distal bone
	setBoneInfos(m_boneInfos[handIdx].thumbFinger.distalBone,Leap::Bone::invalid ());


	// middle finger	// metacarpal bone
	setBoneInfos(m_boneInfos[handIdx].middleFinger.metacarpalBone,Leap::Bone::invalid ());

	// middle finger	// proximal bone
	setBoneInfos(m_boneInfos[handIdx].middleFinger.ProximalBone,Leap::Bone::invalid ());

	// middle finger	// intermediate bone
	setBoneInfos(m_boneInfos[handIdx].middleFinger.intermediateBone,Leap::Bone::invalid ());

	// middle finger	// distal bone
	setBoneInfos(m_boneInfos[handIdx].middleFinger.distalBone,Leap::Bone::invalid ());


	// pinky finger	// metacarpal bone
	setBoneInfos(m_boneInfos[handIdx].pinkyFinger.metacarpalBone,Leap::Bone::invalid ());

	// pinky finger	// proximal bone
	setBoneInfos(m_boneInfos[handIdx].pinkyFinger.ProximalBone,Leap::Bone::invalid ());

	// pinky finger	// intermediate bone
	setBoneInfos(m_boneInfos[handIdx].pinkyFinger.intermediateBone,Leap::Bone::invalid ());

	// pinky finger	// distal bone
	setBoneInfos(m_boneInfos[handIdx].pinkyFinger.distalBone,Leap::Bone::invalid ());


	// ring finger	// metacarpal bone
	setBoneInfos(m_boneInfos[handIdx].ringFinger.metacarpalBone,Leap::Bone::invalid ());

	// ring finger	// proximal bone
	setBoneInfos(m_boneInfos[handIdx].ringFinger.ProximalBone,Leap::Bone::invalid ());

	// ring finger	// intermediate bone
	setBoneInfos(m_boneInfos[handIdx].ringFinger.intermediateBone,Leap::Bone::invalid ());

	// ring finger	// distal bone
	setBoneInfos(m_boneInfos[handIdx].ringFinger.distalBone,Leap::Bone::invalid ());
}
