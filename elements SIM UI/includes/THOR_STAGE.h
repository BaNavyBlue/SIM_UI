#ifndef THOR_STAGE_H
#define THOR_STAGE_H

#include <SIM_UI_Headers.hpp>
#include <Thorlabs.MotionControl.KCube.DCServo.h>

const int REVOLUTIONS = 512; // steps/counts per rotation
const double _one_mm = 67.49; // rotations
const double _target_velocity = 2.3; // mm/s
const double _target_acceleration = 2.5; // mm/s^2
const double _homing_velocity_mm_s = 1.0; // mm/s
const int one_mm_in_counts = 34555;
const double T = 2048 / 6000000.0;
const int A_V_val = 65536;
const double minPos_mm = 0.0;
const double maxPos_mm = 25.0; // Limited by stage range

class THOR_STAGE
{
public:
	int THOR_STAGE_THD(void* data_ptr);
	void moveRel(double displace);
	void moveAbs(double displace);
	bool waitForMove();
	bool waitForHome();
	void runSevenPhaseSeq();
	bool threadIsAlive = false;
	bool axial_stage_seq_run = false;
	double position_mm = 0.0;
	int position_counts = 0;
	double blue_axial_shift = 2.1608;// 2.1608;// 1.9323; // 2.2608;// 2.632;//2.81;
	STAGE_THREAD_DATA* data;
	// Change this line to reflect your device's serial number
	int serialNo = 27602274;
	char testSerialNo[16];

	double smallStep = 0.1;
	double largeStep = 1.0;
private:

	
	int minPos_counts = 1;
	int maxPos_counts = maxPos_mm*one_mm_in_counts;
	int velocity = one_mm_in_counts * T * A_V_val * _target_velocity;
	int acceleration = one_mm_in_counts * T * T * A_V_val * _target_acceleration;
	unsigned int homingVel = one_mm_in_counts * T * A_V_val * _homing_velocity_mm_s;


	std::thread* stage_seq_thd_ptr = nullptr;
	std::mutex command_protect;
};

#endif