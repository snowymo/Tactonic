#ifndef _AR_GLASS_LISTENER_H
#define _AR_GLASS_LISTENER_H

#include <leap.h>
#include "LeapMotionData.h"
#include <iostream>
//#include <glm/glm.hpp>
//#include "glm/glm.hpp"
#include "D:\Libs\glm\0.9.3.4\glm\glm.hpp"

enum LeapMotionAction{
	NONE,
	MENU,
	CLICK,
	// 2014-5-24 15:27:18
	TRANS,
	SCALE,
	ROTATE,
	// 2014-5-24 16:13:47
	//SINGLE,
	DOUBLE
};

// ���ﳢ��һ��struct ���C#��ͷдһ���ľͿ����� vector����ʱ��ArrayListһ�±���
struct ARGlassAction 
{
	LeapMotionAction name;	// ���Ǹ�ö����
	LeapMotionAction nameOld;	// ���Ǹ�ö����
	Leap::Vector leapMotionPos;		// vector������float���
	Leap::Vector leapMotionPosOld;		// vector������float���
	Leap::Vector leapMotionRightPos;		// vector������float��� doubleר��
	Leap::Vector leapMotionPosRightOld;		// vector������float��� doubleר��
	Leap::Vector transVec;
	float scaleVec;
	Leap::Vector rotateVec;
	Leap::Vector camTransPos;		// �����任֮�������������� unused
	float age;
};

// ���������е�����״̬����
// enum LeapMotionStatus
// {
// 	MENU,
// 	TRANS,
// 	SCALE,
// 	ROTATE,
// 	DOUBLE
// };

class ARGlassListener :
	public Leap::Listener
{
private:
	ARGlassListener();
	~ARGlassListener();

public:
	void addListener();
	void removeListener();

public:

	//************************************
	// Method:    onInit
	// FullName:  ARGlassListener::onInit
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onInit(const Leap::Controller&);
	//************************************
	// Method:    onConnect
	// FullName:  ARGlassListener::onConnect
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onConnect(const Leap::Controller& controller);
	//************************************
	// Method:    onDisconnect
	// FullName:  ARGlassListener::onDisconnect
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onDisconnect(const Leap::Controller&);
	//************************************
	// Method:    onExit
	// FullName:  ARGlassListener::onExit
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onExit(const Leap::Controller&);
	//************************************
	// Method:    onFrame
	// FullName:  ARGlassListener::onFrame
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onFrame(const Leap::Controller& controller);
	//************************************
	// Method:    onFocusGained
	// FullName:  ARGlassListener::onFocusGained
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onFocusGained(const Leap::Controller&);
	//************************************
	// Method:    onFocusLost
	// FullName:  ARGlassListener::onFocusLost
	// Access:    virtual public 
	// Returns:   void
	// Qualifier:
	// Parameter: const Leap::Controller &
	//************************************
	virtual void onFocusLost(const Leap::Controller&);
	virtual void onDeviceChange(const Leap::Controller&);
	virtual void onServiceConnect(const Leap::Controller&);
	virtual void onServiceDisconnect(const Leap::Controller&);

	public:
		static ARGlassListener* getInstance();

		LeapMotionData * getLeapMotionData() const { return m_pLeapMotionData; }
		void setLeapMotionData(LeapMotionData * val) { m_pLeapMotionData = val; }
		glm::mat4x4 getTransformation() const { return m_transformation; }
		void setTransformation(glm::mat4x4 val) { m_transformation = val; }

		bool getMenuAction(ARGlassAction * action);
		bool getSingleAction(ARGlassAction * action, LeapMotionAction status);
		bool getDoubleAction(ARGlassAction * action, LeapMotionAction status);
		void calCamposFromLeappos(ARGlassAction * action, int index);

		Leap::Controller getCtrl() const { return m_ctrl; }
		void Ctrl(Leap::Controller val) { m_ctrl = val; }

	private:
		static ARGlassListener * m_pInstance;
		Leap::Controller m_ctrl;
		
		LeapMotionData * m_pLeapMotionData;		
		glm::mat4x4 m_transformation;
		
private:
	// ��Ҫ֪����ǰ������ ÿ�����ƶ�Ӧ�Ĺؼ����λ�� ��˵����Ƶ�����λ�� �������Ƶ�ָ��λ��
	bool testGetAction(ARGlassAction  * menuAction,int* menuActionCnt,
		ARGlassAction  * clickAction,int* clickActionCnt,
		long* frameId);
	// ���ݵ�ǰ�������Ƶ�״̬���� �õ����� ��������Ĭ��Ϊ1 ������������˵����Ƶ�����λ�� RST���ƶ�λ��
	bool testGetAction(ARGlassAction * action,long * frameId, bool * isAction, LeapMotionAction status);
	ARGlassAction * menuAction;
	long * fid;
	// �������ƺͼ�¼�µ�һ��������vec ������Ӧ��rst
	void calculateVec(ARGlassAction * action,LeapMotionAction status, Leap::Vector refVec, Leap::Vector refVec2 = Leap::Vector());
	void calculateRotateVec(ARGlassAction * action,Leap::Vector refVec);
	void calculateDoubleVec( ARGlassAction * action );
	//LeapMotionAction status2arglass(LeapMotionStatus status);
};

#endif