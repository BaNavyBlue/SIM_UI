#ifndef STAGE_CONTROL_H
#define STAGE_CONTROL_H
#include "SIM_UI_Headers.hpp"
#include "PI_Stage.h"
#include <THOR_STAGE.h>


class Stage{
public:
	//Stage(stage_type);
	bool start_stage(stage_type stage_t);
	void join_stage_thread();
	STAGE_THREAD_DATA* data;
	PI_STAGE PIStage;
	THOR_STAGE THORStage;
	bool thread_started = false;

private:
	std::thread* stage_thread_ptr;
	int iD;
};


#endif