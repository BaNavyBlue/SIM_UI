

#ifndef PI_STAGE_H
#define PI_STAGE_H
#include <SIM_UI_Headers.hpp>
#include "PI_GCS2_DLL.h"

const BOOL SERVO_ON = TRUE;
const BOOL SERVO_OFF = FALSE;


enum ConnectionType
{
	Dialog,
	RS232,
	TCPIP,
	USB
};

class PI_STAGE {
public:

	double position;
	bool threadIsAlive = false;
	bool trigger_thd_run = false;
	int PIStage_Thread(void* data_ptr);
	void queryPos();
	void moveRel(double displace);
	void moveAbs(double newPos);
	void runSequence();
	double smallStep = 0.1;
	double largeStep = 1.0;
	STAGE_THREAD_DATA* data;


	int iD;
	std::string sAxis;
	const int piChannelsArray[2] = { 1,2 };
	BOOL pbValueArray[2] = { false, false };
	int iArraySize = 2;
private:
	
	std::thread* trigger_thread_ptr = nullptr;
	std::mutex command_protect;
	


	double minPos;
	double maxPos;
	double range;
	double returnPos;
	void OpenConnection(ConnectionType type, std::string hostName);
	void ReportError(int iD);
	bool WaitForMotionDone(int ID, std::string axis);
	bool WaitForTrigger(int ID, const int* channels, BOOL* values, int numChan, bool* running);
	void PrintControllerIdentification(int iD);
	void CloseConnectionWithComment(int iD, std::string comment);
	double GetMaxPositionLimit(int ID, std::string axis);
	double GetMinPositionLimit(int ID, std::string axis);
	void SetServoState(int ID, std::string axis, BOOL state);
};


#endif