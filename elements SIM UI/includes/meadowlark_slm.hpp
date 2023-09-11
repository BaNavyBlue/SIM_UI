#ifndef MEADOWLARK_HPP
#define MEADOWLARK_HPP
#include <SIM_UI_Headers.hpp>
//#include "SIM_UI.hpp"
#include "Blink_C_wrapper.h"  // Relative path to SDK header.
#include "ImageGen.h"


class meadowlark {
public:
   //bool start_meadowlark();
   void disable_meadowlark();
   int slm_thread(void* data);

   SLM_THREAD_DATA* data;
   bool keep_alive = false;
   bool is_running = false;
   char lutPath[256];
   char stripePath3BEAM[256];
   char stripePath7Phase[256];
   char stripePath3BEAMGREEN[256];
   char stripePath7PhaseGREEN[256];
   char stripePath3BEAMTWO[256];
   char stripePath7PhaseTWO[256];
   std::string stPath;
   int load_images();
   bool reloadImageBuffer();


private:
   void get_paths();
   void load_lut();

   int board_number;
   
   // Construct a Blink_SDK instance
   unsigned int bits_per_pixel = 12U;   //12U is used for 1920x1152 SLM, 8U used for the small 512x512
   bool         is_nematic_type = true;
   bool         RAM_write_enable = true;
   bool         use_GPU_if_available = true;
   unsigned int n_boards_found = 0U;
   int         constructed_okay = true;
   bool ExternalTrigger = true;
   bool FlipImmediate = false; //only supported on the 1k

   //Both pulse options can be false, but only one can be true. You either generate a pulse when the new image begins loading to the SLM
   //or every 1.184 ms on SLM refresh boundaries, or if both are false no output pulse is generated.
   bool OutputPulseImageFlip = true;
   bool OutputPulseImageRefresh = false; //only supported on 1920x1152, FW rev 1.8.

   int height;
   int width;
   int depth; //bits per pixel
   int Bytes;
   int ImgSize;
   unsigned int buffer_size;



   uint8_t* im_buffer; //dynamic image buffer
   uint8_t* im_ptr; //buffer image pointer

   //bool is_running; // For SLM thread
   uint32_t offset; // For image buffer stride

   std::mutex crit_buffer;
};

#endif
