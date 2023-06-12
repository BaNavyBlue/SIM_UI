#include <SLM.hpp>


void SLM_Interface::start_meadowlark()
{
   auto make_slm_thread = [&]() {
      thread_started = true;
      if (SLM.slm_thread(data)) {
         std::cout << "something bad" << std::endl;
      }
   };
   slm_thread_ptr = new std::thread(make_slm_thread);
}

void SLM_Interface::join_slm_thread() {
   if (slm_thread_ptr->joinable()) {
      std::cout << "thread is joinable" << std::endl;
      slm_thread_ptr->join();
   }
   else {
      delete[]slm_thread_ptr;
   }
}
