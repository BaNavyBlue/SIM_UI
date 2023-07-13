


#ifndef SIM_UI_HPP
#define SIM_UI_HPP

#include "SIM_UI_Headers.hpp"
#include "USB_INTERFACE.h"
#include "SLM.hpp"
#include "Stage_control.h"

enum TABS
{
   INTERFACE_TAB,
   TIMING_TAB,
   SLM_TAB,
   STAGE_CONTROL
};

//using namespace cycfi::elements;
namespace el = cycfi::elements;
class SIM_UI;

class SIM_UI : public app {
public:
   auto make_tabs(el::view& view_);
   void start_test_thread();
   using toggle_ptr = std::shared_ptr<el::layered_button>;

   //using radio_b_ptr = std::shared_ptr<el::basic_toggle_button>;
   //using label_ptr = decltype(el::share(el::label("SIM_UI Aligator")));
   
   void refresh_view();
   SIM_UI(int argc, char* argv[]);
   ~SIM_UI();

   SLM_THREAD_DATA SLM_DATA;
   STAGE_THREAD_DATA PI_DATA;
   STAGE_THREAD_DATA THOR_DATA;
   USB_THREAD_DATA USB_DATA;
   USB_INTERFACE USB;
   bool PI_trigger_stage = false;
   bool PI_trigger_mode1 = true;
   bool PI_trigger_mode2 = false;
   bool PI_center_range = true;
   //std::mutex sleep_PI;
   //std::condition_variable signal_PI;

private:
   auto make_basic_interface();
   auto make_basic_text2();
   auto make_elements(TABS tab_value);
   auto make_stage_tab();
   void start_slm();
   void start_USB();
   void start_PI_stage();
   void stop_slm();
   void stop_PI_stage();

   toggle_ptr _toggle_blanking;
   
   //label_ptr _stage_pos;
   //label_ptr _msg_label;
   
   //simp_button_ptr _start_latch_ptr;
   bool threads_alive = true;
   //std::unique_ptr<std::thread> toggle_thread;
   std::condition_variable signal_slm;
   std::mutex sleep_slm;

   SLM_Interface SLM;
   Stage PIStage;
   
   bool _PI_STATUS = false;
   bool _SLM_STATUS = false;
   bool _trigger_running = false;
   el::window _win;
   el::view   _view;
   std::string small_step_str = "0.1";
   std::string large_step_str = "1.0";
   std::string jump_to_str = "";
};

#endif SIM_UI_HPP
