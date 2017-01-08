#include <iostream>
//#include "../LeapMotionDll/ExportLMDll.h"
#include "../FrameProcessLeapMotion/ExportLMDll.h"

int main(int argc, char** argv) {
	// Create a sample listener and controller
	//ARGlassListener listener;
	//Leap::Controller controller;

	// Have the sample listener receive events from the controller
	//controller.addListener(listener);
	initLeapMotion ();

	// Keep this process running until Enter is pressed
	std::cout << "Press Enter to quit..." << std::endl;


	//LeapMotionAction * actions = new LeapMotionAction[2];
	//getAction (actions);

	std::cin.get();

	// Remove the sample listener when done
	//controller.removeListener(listener);
	exitLeapMotion ();
	
	//delete [] actions;
	//actions = NULL;

	return 0;
}