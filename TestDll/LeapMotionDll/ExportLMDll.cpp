//ºê¶¨Òå  
#define  EXPORTBUILD_LM

#include "ExportLMDll.h"

Leap::Controller g_controller;
ARGlassListener g_listener;

_DLLExport void LeapMotionInit ()
{
	g_controller.addListener (g_listener)	;
}

_DLLExport void LeapMotionExit ()
{
	g_controller.removeListener (g_listener)	;
}

_DLLExport void processFrame ()
{
//	ARGlassListener::getInstance ()->onFrame (g_controller);
}

_DLLExport void getHandInfo (std::vector<Leap::Vector> & handInfo)
{

}

_DLLExport void getAction (LeapAction * action)
{
	LeapMotionData * leapMotionData = /*ARGlassListener::getInstance()->*/g_listener.getLeapMotionData();
	Leap::HandList myHandList = leapMotionData->getHandlist();
	std::vector<Leap::FingerList> myFingerListaccdHand = leapMotionData->getFingerListaccdHand ();
	for(int i = 0; i < myHandList.count (); i++){
		if(myFingerListaccdHand[i].count () >= 4){
			action[i] = LeapAction::MENU;
			std::cout << "hand:\t" << i << "\tdetect MENU action.\n";
		}
// 		else if(myFingerListaccdHand[i].count () == 1){
// 			action[i] = LeapAction::CLICK;
// 			std::cout << "hand:\t" << i << "\tdetect CLICK action.\n";
// 		}
	}
}
