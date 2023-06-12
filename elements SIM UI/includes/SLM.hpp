

#ifndef SLM_HPP
#define SLM_HPP

#include "SIM_UI_Headers.hpp"
#include "meadowlark_slm.hpp"


using namespace cycfi::elements;


class SLM_Interface {
public:
   std::string lut_path;
   std::string pattern_path;
   void start_meadowlark();
   void join_slm_thread();
   meadowlark SLM;
   bool thread_started = false;
   SLM_THREAD_DATA* data;

   //void start_thread();

private:
   std::thread* slm_thread_ptr;

};


#endif
