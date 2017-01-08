#include "ARGlassListener.h"

// 
// ARGlassListener::ARGlassListener(void)
// {
// 	m_pLeapMotionData = new LeapMotionData;
// 	Leap::Listener();	
// }
// 
// 
// ARGlassListener::~ARGlassListener(void)
// {
// 	if(m_pInstance)
// 		delete m_pInstance;
// 	m_pInstance = NULL;
// 	
// }

// ARGlassListener* ARGlassListener::getInstance ()
// {
// 	if(NULL == m_pInstance)
// 		m_pInstance = new ARGlassListener;
// 	return m_pInstance;
// }

void ARGlassListener::onInit (const Leap::Controller&)
{
	std::cout << "Initialized" << std::endl;
	OutputDebugStringA("Initialized\n");
	m_sMsg += "Initialized.\n";
}

void ARGlassListener::onConnect (const Leap::Controller& controller)
{
	std::cout << "Connected" << std::endl;
	m_sMsg += "Connected.\n";
	OutputDebugStringA("Connected\n");
	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void ARGlassListener::onDisconnect (const Leap::Controller& controller)
{
	std::cout << "Disconnected" << std::endl;
	m_sMsg += "Disconnected.\n";
	OutputDebugStringA("Disconnected\n");
}

void ARGlassListener::onExit (const Leap::Controller& controller)
{
	std::cout << "Exited" << std::endl;
	m_sMsg += "Exited.\n";
	OutputDebugStringA("Exited\n");
}

void ARGlassListener::onFocusGained (const Leap::Controller& controller)
{
	std::cout << "Focus Gained" << std::endl;
	m_sMsg += "Focus Gained.\n";
	OutputDebugStringA("Focus Gained\n");
}

void ARGlassListener::onFocusLost (const Leap::Controller& controller)
{
	std::cout << "Focus Lost" << std::endl;
	m_sMsg += "Focus Lost.\n";
	OutputDebugStringA("Focus Lost\n");
}

//************************************
// Method:    onFrame
// FullName:  ARGlassListener::onFrame
// Access:    virtual public 
// Returns:   void
// Qualifier:
// Parameter: const Leap::Controller & controller
//************************************
void ARGlassListener::onFrame (const Leap::Controller& controller)
{
	// get current frame
	const Leap::Frame frame = controller.frame();

	// hands
	Leap::HandList handList = frame.hands();
	int handCnt = handList.count();
	m_pLeapMotionData->setHandlist(handList);
	std::cout << "hand count:" << handCnt << "\n";

	// fingers
	Leap::FingerList fingerList = frame.fingers();
	int fingerCnt = fingerList.count();
	m_pLeapMotionData->setFingerList(fingerList);

	// gestures
	Leap::GestureList gesList = frame.gestures();
	int gesCnt = gesList.count();
	m_pLeapMotionData->setGestureList(gesList);

	// interaction box
	m_pLeapMotionData->setInteractionBox(frame.interactionBox());
}

//ARGlassListener* ARGlassListener::m_pInstance = NULL;
