


#ifndef SIM_UI_HEADERS_HPP
#define SIM_UI_HEADERS_HPP
#include "math.h"
#include "stdio.h"
#include <afxwin.h>
#include <WinUser.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <malloc.h>
#include <stdint.h>
#include <elements.hpp>
#include <infra/support.hpp>
#include <thread>
#include "PI_GCS2_DLL.h"
#include <windows.h>
#include <chrono>
#include <sys/types.h>
#include <stdlib.h>
#include <conio.h>
//#include <MMcore.h>
//#include <Configuration.h>
//#include <SIM_UI.hpp>
//#include <SLM.hpp>

//Stuff for USB communication
#define CHANGE_FPS 0x1
#define CHANGE_Z_STEPS 0x2
#define SET_READOUT_SPEED 0x4
#define SLOW_READOUT 0x8
#define SET_LASER_MODE 0x10
#define COUNTING 0x20
#define STOP_COUNT 0x40
#define SET_RUN_MODE 0x100
#define SET_SIM_MODE 0x200
#define START_CAPTURE 0x800
#define STOP_CAPTURE 0x1000
#define STAGE_TRIGG_ENABLE 0x40000
#define STAGE_TRIGG_DISABLE 0x80000
#define SEND_TRIGG 0x100000
#define SET_EXPOSURE 0x200000
#define START_LIVE 0x100000
#define LIVE_RUNNING 0x200000
#define STOP_LIVE 0x400000
#define STAGE_MOVE_COMPLETE 0x800000
#define Z_STACK_RUNNING 0x1000000
#define STOP_Z_STACK 0x2000000
#define TOGGLE_BLANKING 0x4000000
#define TOGGLE_DIG_MOD 0x8000000

#define THREE_BEAM (0x0)
#define TWO_BEAM (0x1)
#define NO_SIM_Z_ONLY (0x2)
#define SINGLE_ANGLE (0x4)
#define SEVEN_PHASE (0x8)

#define FREE_RUN (0x0)
#define Z_MODE (0x1)
#define COUNT_MODE (0x2)
#define ARB_EXP (0x4)

#define STOP_BLANKING (0x0)
#define START_BLANKING (0x1)

#define BLUE_LASER (0u)
#define GREEN_LASER (1u)
#define BOTH_LASERS (2u)



namespace el = cycfi::elements;

enum stage_type {
	PI,
	THOR
};

using label_ptr = decltype(el::share(el::label("-")));
using simp_button_ptr = std::shared_ptr<el::layered_button>;
using input_box_ptr = decltype(el::input_box(""));
//using exp_box_ptr = decltype(el::input_box("0.0"));
//using chk_box_ptr = decltype(el::check_box("Hello"));


struct SLM_THREAD_DATA
{
   el::window* _win;
   el::view* _view;
   //SIM_UI* my_class;
   std::shared_ptr<el::layered_button> _slm_button;
   label_ptr _msg_label;
   std::condition_variable* signal_slm;
   std::mutex* _slm_mutex;
};

struct usb_data {
	float fps;
	float exposure;
	uint32_t flags;
	uint16_t steps;
	uint8_t mode;
	uint8_t bonus;
	uint32_t count;
};

struct USB_THREAD_DATA
{
	usb_data outgoing;
	usb_data incoming;
	bool usb_running = false;
	std::mutex usb_crit;
	std::condition_variable signal_PI;
	std::condition_variable signal_THOR;
	simp_button_ptr start_stop_butt_ptr;
	bool trigger_running = false;
	input_box_ptr fps;
	input_box_ptr exp;
	input_box_ptr lapse_period;
	float fpsVal = 5.0;
	float expVal = 0.0;
	float lapseVal = 20.0;
	int SIM_phases = 5;
	int SIM_angles = 3;
	int min_frames = 15;
	bool arbitrary_exp = false;
	bool count_run_state = true;
	bool triggerStage = false;
	bool run_time_lapse = false;
	uint32_t count = 1;
	int total_image_count = 15;
	int positions = 2; // FOR PI STAGE
	float lapse_time = 5.0; //In minutes
	int lapse_counts = 1;
	label_ptr _config_label;
	//chk_box_ptr* exp_mode_ptr;
	//std::shared_ptr<el::layered_button> _slm_button
};

struct STAGE_THREAD_DATA
{
	el::window* _win;
	el::view* _view;
	label_ptr _pos_label;
	label_ptr _msg_label;
	std::shared_ptr<el::layered_button> _leftS_button;
	std::shared_ptr<el::layered_button> _leftL_button;
	std::shared_ptr<el::layered_button> _RightS_button;
	std::shared_ptr<el::layered_button> _RightL_button;
	double startDisplace = -0.036;
	double endDisplace = 0.036;
	double stepSize = 0.072;
	double returnPos;
	std::condition_variable* signal_stage;
	std::mutex sleep_thread;
	USB_THREAD_DATA* usb_dat;
};

#endif
