#ifndef _ARGLASS_LISTENER_H
#define _ARGLASS_LISTENER_H

#include <windows.h>
#include "Leap.h"
#include "LeapMotionData.h"
#include <string>

class ARGlassListener :
	public Leap::Listener
{
//public:
// 	ARGlassListener(void);
// 	~ARGlassListener(void);

// public:
// 	static ARGlassListener* getInstance();

public:
	std::string getMsg() const { return m_sMsg; }
	void setMsg(std::string val) { m_sMsg = val; }
	LeapMotionData * getLeapMotionData() const { return m_pLeapMotionData; }
	void setLeapMotionData(LeapMotionData * val) { m_pLeapMotionData = val; }

private:
/*	static ARGlassListener* m_pInstance;*/
	LeapMotionData * m_pLeapMotionData;
	
public:
	virtual void onInit(const Leap::Controller&);
	virtual void onConnect(const Leap::Controller& controller);
	virtual void onDisconnect(const Leap::Controller& controller);
	virtual void onExit(const Leap::Controller& controller);
	virtual void onFrame(const Leap::Controller& controller);
	virtual void onFocusGained(const Leap::Controller& controller);
	virtual void onFocusLost(const Leap::Controller& controller);

private:
	std::string m_sMsg;
	
};

#endif