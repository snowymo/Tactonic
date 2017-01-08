#define EXPORTBUILD_LEAP_MOTION
// newest 2014-4-4 15:38:47

#include "ExportLMDll.h"
#include <fstream>

std::fstream outputfile;

// extern "C" bool _DLLExport getAction (ARGlassAction * menuAction,int* menuActionCnt, ARGlassAction * clickAction,int* clickActionCnt, long* frameId)
// {
// 	//得先确认是最新的帧 不然每次都返回一样的东西 但是leapMotion没有刷新 我先试试不修改 // 一帧处理过后
// 	LeapMotionData * leapMotionData = ARGlassListener::getInstance()->getLeapMotionData();
// 	long fid = leapMotionData->getFrameid ();
// 	//outputfile << "old fid: " << frameId[0] << "\tnew fid:" << fid << "\n";
// 	if(fid == frameId[0]){
// 		return false;
// 		//outputfile.close ();
// 	}
// 	else
// 		frameId[0] = fid;
// 
// 	outputfile.open ("log.txt",std::ios_base::app);
// 	// 初始化 name的old赋值和cnt归零
// 	Leap::HandList myHandList = leapMotionData->getHandlist();
// 	int handCount = myHandList.count () > 2 ? 2 : myHandList.count ();
// 	//outputfile << "hand cnt: " << handCount << "\n";
// 	menuActionCnt[0] = 0;
// 	clickActionCnt[0] = 0;
// 	for(int i = 0; i < 2; i++){
// 		menuAction[i].nameOld = menuAction[i].name;
// 		menuAction[i].name = NONE;
// 		clickAction[i].nameOld = clickAction[i].name;
// 		clickAction[i].name = NONE;
// 	}
// 
// 	for(int i = 0; i < handCount; i++){
// 		std::vector<Leap::FingerList> myFingerListaccdHand = leapMotionData->getFingerListaccdHand ();
// 		//outputfile << "finger cnt: " << myFingerListaccdHand[i].count () << "\n";
// 
// 		if(myFingerListaccdHand[i].count () >= 4){
// 			menuAction[i].name = LeapMotionAction::MENU;
// 			menuAction[i].leapMotionPos = myHandList[i].palmPosition();
// 			++menuActionCnt[0];
// 			outputfile << "hand:\t" << i << "\tdetect MENU action.\tCount:\t" << menuActionCnt[0] 
// 			<< "\tpos:" << menuAction[i].leapMotionPos << std::endl;			
// 			calCamposFromLeappos (menuAction,i);
// 			outputfile <<  "\tcam pos:" << menuAction[i].camTransPos << std::endl;			
// 		}
// 		else if(myFingerListaccdHand[i].count () == 1){
// 			clickAction[i].name = LeapMotionAction::CLICK;
// 			clickAction[i].leapMotionPos = myFingerListaccdHand[i][0].tipPosition ();
// 			++clickActionCnt[0];
// 			outputfile << "hand:\t" << i << "\tdetect CLICK action.\nCount:\t" << clickActionCnt[0]
// 			<< "\tpos:" << clickAction[i].leapMotionPos << std::endl;
// 			calCamposFromLeappos (clickAction,i);
// 			outputfile << "\tcam pos:" << clickAction[i].camTransPos << std::endl;
// 		}		
// 	}
// 
// 	// 根据click手势的左右进行调整 x小的index小
// 	if(clickAction[0].name == clickAction[1].name){
// 		if(clickAction[0].name == LeapMotionAction::CLICK){
// 			if(clickAction[0].camTransPos.x < clickAction[1].camTransPos.x){
// 				// exchange
// 				Leap::Vector lpTemp;
// 				lpTemp = clickAction[0].camTransPos;
// 				clickAction[0].camTransPos = clickAction[1].camTransPos;
// 				clickAction[1].camTransPos = lpTemp;
// 				lpTemp = clickAction[0].camTransPos;
// 				clickAction[0].leapMotionPos = clickAction[1].leapMotionPos;
// 				clickAction[1].leapMotionPos = lpTemp;
// 				float fTemp;
// 				fTemp = clickAction[0].age;
// 				clickAction[0].age = clickAction[1].age;
// 				clickAction[1].age = fTemp;
// 			}
// 		}
// 	}
// 	outputfile.close ();
// 	return true;
// }

extern "C" bool _DLLExport getAction (ARGlassAction * action,LeapHand * bondInfos,long * frameId, bool * isAction,  LeapMotionAction status)
{
	//得先确认是最新的帧 不然每次都返回一样的东西 但是leapMotion没有刷新 我先试试不修改 // 一帧处理过后
	LeapMotionData * leapMotionData = ARGlassListener::getInstance()->getLeapMotionData();
	long fid = leapMotionData->getFrameid ();
	//std::cout << "old fid: " << frameId[0] << "new fid:" << fid << "\n";
	if(fid == frameId[0]){
		return false;
	}
	else
		frameId[0] = fid;

	// 初始化 name的old赋值和cnt归零
	action[0].nameOld = action[0].name;
	action[0].name = NONE;
	action[0].leapMotionPosOld = action[0].leapMotionPos;
	action[0].leapMotionPosRightOld = action[0].leapMotionRightPos;
	action[0].rotateVec = action[0].transVec = Leap::Vector();
	action[0].scaleVec = 1;

	leapMotionData->getBoneInfos (bondInfos);
	//bool result = false;
	// 菜单为任意时刻都可召唤
	isAction[0] = ARGlassListener::getInstance ()->getMenuAction (action);
	if(isAction[0])
		return true;

	// 根据状态进行识别
	switch (status)
	{
// 	case MENU:
// 		isAction[0] = ARGlassListener::getInstance ()->getMenuAction (action);
// 		//result = getMenuAction (action);
// 		break;
	case TRANS:
	case SCALE:
	case ROTATE:
		isAction[0] = ARGlassListener::getInstance ()->getSingleAction (action,status);
		break;
	case DOUBLE:
		isAction[0] = ARGlassListener::getInstance ()->getDoubleAction (action,status);
		break;
	default:
		break;
	}
	return true;
}

extern "C" void _DLLExport initLeapMotion ()
{
	ARGlassListener::getInstance ()->addListener ();
}

extern "C" void _DLLExport exitLeapMotion ()
{
	ARGlassListener::getInstance ()->removeListener ();
}

void calCamposFromLeappos (ARGlassAction * action, int index)
{
	// hehe 2014-4-28 23:43:16 利用变换矩阵把三维leap坐标→cam坐标
	// hehe 2014-5-8 14:29:49 左右相反
	glm::vec4 leapPos = glm::vec4(action[index].leapMotionPos.x,
		action[index].leapMotionPos.y,
		action[index].leapMotionPos.z,
		1),
		camPos;
	// 变换之后毫无反应
	glm::mat4x4 transform = ARGlassListener::getInstance()->getTransformation();
	camPos = leapPos * transform;
	//std::cout << "leap pos: " << action[index].leapMotionPos.x << "\t" << leapPos.y << "\t" << leapPos.z << "\n";
	//std::cout << "cam pos: " << camPos.x << "\t" << camPos.y << "\t" << camPos.z << "\t" << camPos.w << "\n";
	action[index].camTransPos.x = camPos[1];
	action[index].camTransPos.y = camPos[0];
	action[index].camTransPos.z = camPos[2];
}

extern "C" void _DLLExport init ()
{
	ARGlassListener::getInstance ()->addListener ();
}
