#include "ARGlassListener.h"
#include <fstream>
#include <time.h>

#define _TEST_DLL

void ARGlassListener::onInit(const Leap::Controller& controller) {
	std::fstream outputfile;
	outputfile.open ("log.txt",std::ios_base::app);
	std::cout << "Initialized" << std::endl;
	std::fstream transFile;
	std::string str;
	transFile.open ("transformation.txt");
	if(!transFile.fail ()){
		std::getline (transFile,str);
		std::getline (transFile,str);
		std::getline (transFile,str);
		std::getline (transFile,str);
		std::getline (transFile,str);
		std::getline (transFile,str);
		double d;
		for(int i = 0; i < 16; i++){
			transFile >> d;
			m_transformation[i/4][i%4] = d;
			//outputfile << d << "\t";
		}
	}	
	transFile.close ();
	
	outputfile << "\nlog:\tinit\n";
	outputfile.close ();

	menuAction = new ARGlassAction [2];
	fid = new long(0);
}

void ARGlassListener::onConnect(const Leap::Controller& controller) {
	std::cout << "Connected" << std::endl;
 	controller.enableGesture(Leap::Gesture::TYPE_CIRCLE);
// 	controller.enableGesture(Leap::Gesture::TYPE_KEY_TAP);
// 	controller.enableGesture(Leap::Gesture::TYPE_SCREEN_TAP);
// 	controller.enableGesture(Leap::Gesture::TYPE_SWIPE);
}

void ARGlassListener::onDisconnect(const Leap::Controller& controller) {
	//Note: not dispatched when running in a debugger.
	std::cout << "Disconnected" << std::endl;
}

void ARGlassListener::onExit(const Leap::Controller& controller) {
	std::cout << "Exited" << std::endl;

	if(menuAction)
		delete [] menuAction;
	menuAction = NULL;
}

void ARGlassListener::onFrame(const Leap::Controller& controller) {
	// Get the most recent frame and report some basic information
	// get current frame
	const Leap::Frame frame = controller.frame();
	float fps = frame.currentFramesPerSecond ();
	
	std::fstream outputfile;
	outputfile.open ("log.txt",std::ios_base::app);
	outputfile << "timestamp: " << frame.timestamp()
		/*<< "time:\t" << time (NULL) */
		<< "\tframe id:\t" << frame.id ()
		<< "\tfps:\t" << fps 
		<< "\thands:\t" << frame.hands().count()
		<< "\tfingers:\t" << frame.fingers().count()
		<< "\n";
	outputfile.close ();
// 	std::cout << "Leap::Frame id: " << frame.id()
// 		<< ", timestamp: " << frame.timestamp()
// 		<< ", hands: " << frame.hands().count()
// 		<< ", fingers: " << frame.fingers().count()
// 		<< ", tools: " << frame.tools().count()
// 		<< ", gestures: " << frame.gestures().count() << std::endl;

	// id
	m_pLeapMotionData->setFrameid (frame.id ());

	// hands
	Leap::HandList handList = frame.hands();
	int handCnt = handList.count();
	m_pLeapMotionData->setHandlist(handList);

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

	// 2014-5-28 21:29:32 bone infos
	m_pLeapMotionData->setBoneInfos (handList);

	// test method
#ifdef _TEST_DLL
// 	ARGlassAction * menuAction = new ARGlassAction [2],
// 		*clickAction = new ARGlassAction[2];
// 	int menuActionCnt = 0,clickActionCnt = 0;
	bool isAction;
	testGetAction (menuAction,fid,&isAction,LeapMotionAction::DOUBLE);
	std::cout << "foundAction:" << isAction << "\tname:\t" << menuAction[0].name << "\n";
	std::cout << "ref vec:\t rotate:\t" << menuAction[0].rotateVec
		<< "\tscale:\t" << menuAction[0].scaleVec
		<< "\ttrans:\t" << menuAction[0].transVec << "\n";
	
#endif // _DEBUG
}

void ARGlassListener::onFocusGained(const Leap::Controller& controller) {
	std::cout << "Focus Gained" << std::endl;
}

void ARGlassListener::onFocusLost(const Leap::Controller& controller) {
	std::cout << "Focus Lost" << std::endl;
}

ARGlassListener * ARGlassListener::m_pInstance = NULL;

ARGlassListener* ARGlassListener::getInstance ()
{
	if(m_pInstance == NULL)
		m_pInstance = new ARGlassListener;
	return m_pInstance;
}

void ARGlassListener::addListener ()
{
	m_ctrl.addListener (*m_pInstance);
}

void ARGlassListener::removeListener ()
{
	m_ctrl.removeListener (*m_pInstance);
}

ARGlassListener::ARGlassListener ()
{
	m_pLeapMotionData = new LeapMotionData;
}

ARGlassListener::~ARGlassListener ()
{
	if(m_pLeapMotionData != NULL)
		delete m_pLeapMotionData;
	m_pLeapMotionData = NULL;
}

bool ARGlassListener::testGetAction (ARGlassAction  * menuAction,int* menuActionCnt,
									 ARGlassAction  * clickAction,int* clickActionCnt,
									 long* frameId)
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
	Leap::HandList myHandList = leapMotionData->getHandlist();
	int handCount = myHandList.count () > 2 ? 2 : myHandList.count ();
	//std::cout<< "hand cnt: " << handCount << "\n";
	menuActionCnt[0] = 0;
	//clickActionCnt[0] = 0;
	for(int i = 0; i < 2; i++){
		menuAction[i].nameOld = menuAction[i].name;
		menuAction[i].name = NONE;
		//clickAction[i].nameOld = clickAction[i].name;
		//clickAction[i].name = NONE;
	}

	for(int i = 0; i < handCount; i++){
		std::vector<Leap::FingerList> myFingerListaccdHand = leapMotionData->getFingerListaccdHand ();
		//std::cout << "finger cnt: " << myFingerListaccdHand[i].count () << "\n";

		if(myFingerListaccdHand[i].count () >= 4){
			menuAction[i].name = LeapMotionAction::MENU;
			menuAction[i].leapMotionPos = myHandList[i].palmPosition();
			++menuActionCnt[0];
			std::cout << "hand:\t" << i << "\tdetect MENU action.\nCount:\t" << menuActionCnt[0] << std::endl;			
			calCamposFromLeappos (menuAction,i);
		}
// 		else if(myFingerListaccdHand[i].count () == 1){
// 			clickAction[i].name = LeapMotionAction::CLICK;
// 			clickAction[i].leapMotionPos = myFingerListaccdHand[i][0].tipPosition ();
// 			//++clickActionCnt[0];
// 			//std::cout << "hand:\t" << i << "\tdetect CLICK action.\nCount:\t" << clickActionCnt[0] << std::endl;
// 			calCamposFromLeappos (clickAction,i);			
// 		}		
	}
	
	return true;
}

//逻辑上就是 根据不同的状态检测不同的手势
bool ARGlassListener::testGetAction (ARGlassAction * action,long * frameId, bool * isAction,  LeapMotionAction status)
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

void ARGlassListener::calCamposFromLeappos (ARGlassAction * action, int index)
{
	// hehe 2014-4-28 23:43:16 利用变换矩阵把三维leap坐标→cam坐标
	// hehe 2014-5-8 14:29:49 左右相反
	glm::vec4 leapPos = glm::vec4(-action[index].leapMotionPos.x,
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

bool ARGlassListener::getMenuAction (ARGlassAction * action)
{
	LeapMotionData * leapMotionData = ARGlassListener::getInstance()->getLeapMotionData();
	Leap::HandList myHandList = leapMotionData->getHandlist();
	int handCount = myHandList.count ();
	std::cout<< "menu action hand cnt: " << handCount << "\n";

	for(int i = 0; i < handCount; i++){
		Leap::FingerList fingerList = myHandList[i].fingers();
		//std::cout << "finger cnt: " << fingerList.count () << "\n";

		if(fingerList.count () >= 4){
			// 判断手指的姿势 必须伸直
			int validFingerCnt = 0;
			for (Leap::FingerList::const_iterator fl = fingerList.begin(); fl != fingerList.end(); ++fl){
				const Leap::Finger finger = *fl;
 				Leap::Vector boneStart = finger.bone(static_cast<Leap::Bone::Type>(0)).nextJoint ();
 				Leap::Vector boneEnd = finger.bone(static_cast<Leap::Bone::Type>(3)).nextJoint ();
// 				Leap::Vector fingerLength = boneEnd - boneStart;
// 				if((finger.length () * 2 <= fingerLength.magnitude ())
// 					&& finger.length () * 3.5 >= fingerLength.magnitude ()){
// 						++validFingerCnt;
// 						std::cout << "validFingerCnt:" << validFingerCnt << "\n";
// 				}
				// 2014-6-9 12:15:20 算骨头长度 
				float boneLength1 = finger.bone(static_cast<Leap::Bone::Type>(0)).length ();
// 				float boneLength2 = finger.bone(static_cast<Leap::Bone::Type>(1)).length ();
// 				float boneLength3 = finger.bone(static_cast<Leap::Bone::Type>(2)).length ();
// 				float boneLength4 = finger.bone(static_cast<Leap::Bone::Type>(3)).length ();
				if((boneEnd - boneStart).magnitude () > 1.08 * boneLength1){
					++validFingerCnt;
				}
				//std::cout << "2to4.length ()/boneLength1:" << (boneEnd - boneStart).magnitude ()/boneLength1 << "\n";
				//std::cout << "boneLength1:" << boneLength1 << "\n";
				
			}
			std::cout << "validFingerCnt:" << validFingerCnt << "\n";
			if(validFingerCnt >= 4){
				action[i].name = LeapMotionAction::MENU;
				action[i].leapMotionPos = myHandList[i].palmPosition();
				calCamposFromLeappos (action,i);
				//std::cout << "hand:\t" << i << "\tdetect MENU action.\n";
				// 默认就检测一个菜单手势 先到先得
				return true;
			}	
			else{
				//std::cout << "hand:\t" << i << "\tdetect NO action.\n";
			}
		}	
	}
	return false;
}

bool ARGlassListener::getSingleAction (ARGlassAction * action, LeapMotionAction status)
{
	// 单手手势 也就是一只手做平移来表示rst 稳健起见
	LeapMotionData * leapMotionData = ARGlassListener::getInstance()->getLeapMotionData();
	Leap::HandList myHandList = leapMotionData->getHandlist();
	int handCount = myHandList.count ();
	//std::cout<< "single action hand cnt: " << handCount << "\n";
	if(handCount != 1)
		return false;
	for(int i = 0; i < handCount; i++){
		Leap::FingerList fingerList = myHandList[i].fingers();
		//std::cout << "finger cnt: " << fingerList.count () << "\n";

		if(fingerList.count () >= 4){
			// 判断手指的姿势 必须伸直
			int validFingerCnt = 0;
			int validFingerIdx = 0;
			//for (Leap::FingerList::const_iterator fl = fingerList.begin(); fl != fingerList.end(); ++fl,validFingerIdx++){
			for(int idx = 0; idx < fingerList.count (); idx++){
				const Leap::Finger finger = fingerList[idx];
				if(finger.type () == Leap::Finger::TYPE_THUMB)
					continue;
				Leap::Vector boneStart = finger.bone(static_cast<Leap::Bone::Type>(0)).nextJoint ();
				Leap::Vector boneEnd = finger.bone(static_cast<Leap::Bone::Type>(3)).nextJoint ();
				// 2014-6-9 12:15:20 算骨头长度 
				float boneLength1 = finger.bone(static_cast<Leap::Bone::Type>(0)).length ();
				if((boneEnd - boneStart).magnitude () > 1.08 * boneLength1){
					++validFingerCnt;
					//std::cout << "validFingerCnt:" << validFingerCnt << "\n";
				}
			}
			if(validFingerCnt == 1){
				action[0].name = /*status2arglass*/ status;
				action[0].leapMotionPos = fingerList[validFingerIdx].tipPosition ();
				// 2014-6-9 12:07:52 add
				action[0].leapMotionPos = myHandList[i].palmPosition();
				std::cout << "hand:\t" << i << "\tdetect SINGLE action.\n";
				// 单手手势就检测一个 先到先得
				if(action[0].leapMotionPosOld == Leap::Vector())
					return false;
				calculateVec (action,status,action[0].leapMotionPos);
				return true;
			}			
			else{
				std::cout << "hand:\t" << i << "\tdetect NO action.\n";
			}
		}	
	}
	return false;
}

bool ARGlassListener::getDoubleAction (ARGlassAction * action, LeapMotionAction status)
{
	// 双手手势 也就是两只手做平移来表示rst 稳健起见
	LeapMotionData * leapMotionData = ARGlassListener::getInstance()->getLeapMotionData();
	Leap::HandList myHandList = leapMotionData->getHandlist();
	int handCount = myHandList.count ();
	//std::cout<< "double action hand cnt: " << handCount << "\n";
	if(handCount != 2)
		return false;
	int validFingerCnt = 0;
	std::vector<int> validFingerIdxs;
	for(int i = 0; i < handCount; i++){
		Leap::FingerList fingerList = myHandList[i].fingers();
		//std::cout << "finger cnt: " << fingerList.count () << "\n";
		int validFingerPerHand = 0;
		int validFingerIdx = -1;
		if(fingerList.count () >= 4){
			// 判断手指的姿势 必须伸直			
			int validFingerIdx = 0;
			for(int idx = 0; idx < fingerList.count (); idx++){
				const Leap::Finger finger = fingerList[idx];
				if(finger.type () == Leap::Finger::TYPE_THUMB)
					continue;
				Leap::Vector boneStart = finger.bone(static_cast<Leap::Bone::Type>(0)).nextJoint ();
				Leap::Vector boneEnd = finger.bone(static_cast<Leap::Bone::Type>(3)).nextJoint ();
				// 2014-6-9 12:15:20 算骨头长度 
				float boneLength1 = finger.bone(static_cast<Leap::Bone::Type>(0)).length ();
				if((boneLength1 != 0) && ((boneEnd - boneStart).magnitude () > 1.08 * boneLength1)){
					++validFingerPerHand;					
					validFingerIdx = idx;
				}
			}
			//std::cout << "hand:" << i << "\tvalidFingerPerHand:" << validFingerPerHand << "\n";
			if(validFingerPerHand == 1){
				validFingerCnt += validFingerPerHand;
				validFingerIdxs.push_back (validFingerIdx);
			}
		}	
	}
	if(validFingerCnt == 2){
		// 把两个有效手指的tip根据左右手放到pos里去
		// 也许palm pos更稳定
		action[0].name = /*status2arglass*/ status;
		if(myHandList[0].isLeft()){
			action[0].leapMotionPos = myHandList[0].fingers()[validFingerIdxs[0]].stabilizedTipPosition();// .tipPosition ();
			action[0].leapMotionRightPos = myHandList[1].fingers()[validFingerIdxs[1]].stabilizedTipPosition ();
			// 2014-6-9 12:08:40
			action[0].leapMotionPos = myHandList[0].palmPosition();
			action[0].leapMotionRightPos = myHandList[1].palmPosition();
		}
		else{
			action[0].leapMotionPos = myHandList[1].fingers()[validFingerIdxs[1]].stabilizedTipPosition ();
			action[0].leapMotionRightPos = myHandList[0].fingers()[validFingerIdxs[0]].stabilizedTipPosition ();
			//
			action[0].leapMotionPos = myHandList[1].palmPosition();
			action[0].leapMotionRightPos = myHandList[0].palmPosition();
		}
		std::cout << "detect DOUBLE action.\n";
		if(action[0].leapMotionPosOld == Leap::Vector())
			return false;
		if(action[0].leapMotionPosRightOld == Leap::Vector())
			return false;
		// 双手手势就检测两个 先到先得
		calculateVec (action,status,action[0].leapMotionPos,action[0].leapMotionRightPos);
		return true;
	}	
	else{
		std::cout << "detect NO action.\n";
	}
	return false;
}

void ARGlassListener::onDeviceChange (const Leap::Controller& controller)
{
	std::cout << "Device Changed" << std::endl;
	const Leap::DeviceList devices = controller.devices();

	for (int i = 0; i < devices.count(); ++i) {
		std::cout << "id: " << devices[i].toString() << std::endl;
		std::cout << "  isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
	}
}

void ARGlassListener::onServiceConnect (const Leap::Controller&)
{
	std::cout << "Service Connected" << std::endl;
}

void ARGlassListener::onServiceDisconnect (const Leap::Controller&)
{
	std::cout << "Service Disconnected" << std::endl;
}

void ARGlassListener::calculateVec (ARGlassAction * action,LeapMotionAction status, Leap::Vector refVec, Leap::Vector refVec2 /*= Leap::Vector()*/)
{
	switch (status)
	{
	case TRANS:
		action[0].transVec = action[0].leapMotionPos - action[0].leapMotionPosOld;
		// 坐标变换 leapMotion:x,y,z→unity:-x,-z,y
		action[0].transVec = Leap::Vector(-action[0].transVec.x,action[0].transVec.z,-action[0].transVec.y);
		// 把很大的数去掉 通常是错误数据
		if((fabs(action[0].transVec.x) > 50)
			|| (fabs(action[0].transVec.y) > 50)
			|| (fabs(action[0].transVec.z) > 50))
			action[0].transVec = Leap::Vector();
		break;
	case SCALE:
		// 要表示出哪个是放大 x+z
		{
			Leap::Vector deltaV = action[0].leapMotionPos - action[0].leapMotionPosOld;
			action[0].scaleVec = deltaV.magnitude ();
			if((deltaV.x + deltaV.z) > 0)
				action[0].scaleVec *= -1.0f;
			break;
		}
	case ROTATE:
		// 判断是更接近x轴还是y轴 绕x/y旋转 cricle绕z轴旋转
		calculateRotateVec (action,refVec);
		break;
	case DOUBLE:
		// 通过两只手的前后两帧位置判断其rst
		calculateDoubleVec(action);
		break;
	default:
		break;
	}
}

void ARGlassListener::calculateRotateVec (ARGlassAction * action,Leap::Vector refVec)
{
	// 判断circle是否存在
	bool isCircleGesture = false;
	for(int i = 0; i < ARGlassListener::getInstance ()->getLeapMotionData()->getGestureList ().count (); i++){
		if(ARGlassListener::getLeapMotionData()->getGestureList ()[i].type () == Leap::Gesture::TYPE_CIRCLE){
			Leap::CircleGesture circle = ARGlassListener::getLeapMotionData()->getGestureList ()[i];
			// unity 里的 z
			float sweptAngle = 0;
			if (circle.state() != Leap::Gesture::STATE_START) {
				Leap::CircleGesture previousUpdate = 
					Leap::CircleGesture(ARGlassListener::getInstance ()->getCtrl ().frame(1).gesture(circle.id()));
				sweptAngle = (circle.progress() - previousUpdate.progress()) * 2 * Leap::PI;
			}
			if (circle.pointable().direction().angleTo(circle.normal()) <= Leap::PI/4) {
				action[0].rotateVec = Leap::Vector(0,0,sweptAngle);
			} else {
				action[0].rotateVec = Leap::Vector(0,0,-sweptAngle);
			}
			isCircleGesture = true;
		}
	}
	if(!isCircleGesture){
		// 计算delta 在xoy平面为旋转
		Leap::Vector delta = action[0].leapMotionPos - action[0].leapMotionPosOld;
		// 计算旋转轴
		// 就这样了 unity里旋转的时候 就把这个作为轴 长度作为旋转角度比例就好
		// unity里的x y z对应leapMotion的 -x -z y 旋转轴就是-z x 0
		action[0].rotateVec = Leap::Vector(-delta.z,delta.x,0);
	}
}

void ARGlassListener::calculateDoubleVec (ARGlassAction * action)
{
	// 先根据o1o2与n1n2夹角判断是否为旋转
	Leap::Vector o1o2 = action[0].leapMotionPosRightOld - action[0].leapMotionPosOld;
	Leap::Vector n1n2 = action[0].leapMotionRightPos - action[0].leapMotionPos;
	Leap::Vector o1n1 = action[0].leapMotionPos - action[0].leapMotionPosOld;
	Leap::Vector o2n2 = action[0].leapMotionRightPos - action[0].leapMotionPosRightOld;
	float rotateAngle = o1o2.angleTo (n1n2);
	//std::cout << "cal rotate angle:\t" << rotateAngle << "\n";
	float translateRotate,scaleAngle;
	std::fstream outputfile;
	outputfile.open ("log.txt",std::ios_base::app);
	if(rotateAngle <= (Leap::PI/9)){
		// 再根据o1n1与o2n2夹角判断是否为平移
		translateRotate = (o2n2).angleTo (o1n1);
		if(translateRotate < (Leap::PI/9)){
			// translate
			action[0].transVec = (o1n1 + o2n2) / 2.0f;
			// unity.xyz=leapmotion.-x-zy
			action[0].transVec = Leap::Vector(-action[0].transVec.x,-action[0].transVec.z,action[0].transVec.y);
			action[0].name = LeapMotionAction::TRANS;
			outputfile << "trans:" << action[0].transVec << "\n";
		}
		else if(translateRotate > (Leap::PI * 5.0f / 6.0f)){
			scaleAngle = o1o2.angleTo (o1n1);
			std::cout << "cal scaleAngle:\t" << scaleAngle << "\n";
			if(fabs(scaleAngle - Leap::PI/2) > (Leap::PI / 3)){
				// scale
				action[0].scaleVec = n1n2.magnitude () / o1o2.magnitude ();
				action[0].name = LeapMotionAction::SCALE;
				outputfile << "SCALE:" << action[0].scaleVec << "\n";
			}
			else if(fabs(scaleAngle - Leap::PI / 2 ) < (Leap::PI / 6)){
				// rotate
				Leap::Vector rotateAxis = o1o2.cross (n1n2);
				rotateAxis = rotateAxis.normalized ();
				action[0].rotateVec = Leap::Vector(-rotateAxis.x,-rotateAxis.z,rotateAxis.y) * -rotateAngle / Leap::PI * 180.0f;
				// test 在这里把名字改成rotate
				action[0].name = LeapMotionAction::ROTATE;
				outputfile << "ROTATE:" << action[0].rotateVec << "\n";
			}
		}
	}
	outputfile.close ();
}

//LeapMotionAction ARGlassListener::status2arglass (LeapMotionStatus status)
// {
// 	switch (status)
// 	{
// 	case MENU:
// 		return LeapMotionAction::MENU;
// 	case TRANS:
// 		return LeapMotionAction::TRANS;
// 	case SCALE:
// 		return LeapMotionAction::SCALE;
// 	case ROTATE:
// 		return LeapMotionAction::ROTATE;
// 	case DOUBLE:
// 		return LeapMotionAction::DOUBLE;
// 	default:
// 		return LeapMotionAction::NONE;
// 	}
// }
