


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
//#include <SIM_UI.hpp>
//#include <SLM.hpp>

namespace el = cycfi::elements;

using label_ptr = decltype(el::share(el::label("SIM_UI Aligator")));

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


#endif
