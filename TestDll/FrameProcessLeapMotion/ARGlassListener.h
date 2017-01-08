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

// 这里尝试一下struct 这个C#里头写一样的就可以了 vector就暂时和ArrayList一致便是
struct ARGlassAction 
{
	LeapMotionAction name;	// 就是个枚举类
	LeapMotionAction nameOld;	// 就是个枚举类
	Leap::Vector leapMotionPos;		// vector由三个float组成
	Leap::Vector leapMotionPosOld;		// vector由三个float组成
	Leap::Vector leapMotionRightPos;		// vector由三个float组成 double专用
	Leap::Vector leapMotionPosRightOld;		// vector由三个float组成 double专用
	Leap::Vector transVec;
	float scaleVec;
	Leap::Vector rotateVec;
	Leap::Vector camTransPos;		// 经过变换之后的左眼相机坐标 unused
	float age;
};

// 传参类型中的手势状态类型
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
	// 需要知道当前的手势 每个手势对应的关键点的位置 如菜单手势的手掌位置 如点击手势的指尖位置
	bool testGetAction(ARGlassAction  * menuAction,int* menuActionCnt,
		ARGlassAction  * clickAction,int* clickActionCnt,
		long* frameId);
	// 根据当前所需手势的状态出发 得到手势 手势数量默认为1 及操作数比如菜单手势的手掌位置 RST的移动位置
	bool testGetAction(ARGlassAction * action,long * frameId, bool * isAction, LeapMotionAction status);
	ARGlassAction * menuAction;
	long * fid;
	// 根据手势和记录下的一个或两个vec 计算相应的rst
	void calculateVec(ARGlassAction * action,LeapMotionAction status, Leap::Vector refVec, Leap::Vector refVec2 = Leap::Vector());
	void calculateRotateVec(ARGlassAction * action,Leap::Vector refVec);
	void calculateDoubleVec( ARGlassAction * action );
	//LeapMotionAction status2arglass(LeapMotionStatus status);
};

#endif