#include <SIM_UI_HEADERS.hpp>
#include <meadowlark_slm.hpp>
#include "SIM_UI.hpp"

//#include "bmp.h"

#pragma pack(2)
#pragma once

using namespace cycfi::elements;
namespace fs = std::filesystem;
// ------------------------- Blink_SDK_example --------------------------------
// Simple example using the Blink_SDK DLL to send a sequence of phase targets
// to a single SLM.
// To run the example, ensure that Blink_SDK.dll is in the same directory as
// the Blink_SDK_example.exe.
// ----------------------------------------------------------------------------
/*bool meadowlark::start_meadowlark()
{

   // Construct a Blink_SDK instance

   //if bits per pixel is wrong, the lower level code will figure out
   //what it should be and construct properly.
   Create_SDK(bits_per_pixel, &n_boards_found, &constructed_okay, is_nematic_type, RAM_write_enable, use_GPU_if_available, 10U, 0);

   // return of 1 means okay
   if (constructed_okay != 1) {
      std::string temp_s = Get_last_error_message();
      std::wstring stemp = std::wstring(temp_s.begin(), temp_s.end());
      std::cout << stemp.c_str() << std::endl;
      ::AfxMessageBox(stemp.c_str());
      return true;
   }

   // return of 0 means okay, return -1 means error
   if (n_boards_found > 0)
   {
      board_number = 1;
      height = Get_image_height(board_number);
      width = Get_image_width(board_number);
      depth = Get_image_depth(board_number); //bits per pixel
      Bytes = depth / 8;
      ImgSize = height * width * Bytes;
   }
   else {
      return true;
   }
   return false;
}
*/
void meadowlark::get_paths() {
   std::ifstream ifs;
   ifs.open("blink_paths.txt");
   //char lutPath[256];
   //char stripePath[256];
   ifs.getline(lutPath, 256);
   ifs.getline(stripePath, 256);
   ifs.close();

   std::cout << "lutPath: " << lutPath << std::endl;
   std::wcout << "stripePath: " << stripePath << std::endl;
   stPath = stripePath;
}

void meadowlark::load_lut() {
   //***you should replace *_linearVoltage.LUT with your custom LUT file***
  //but for now open a generic LUT that linearly maps input graylevels to output voltages
  //***Using *_linearVoltage.LUT does NOT give a linear phase response***



   if (width == 1920) {
      std::cout << "Load_LUT_file returns: " << Load_LUT_file(board_number, lutPath) << std::endl;
      std::cout << "We in business Boyyyyyyyyy!" << std::endl;
   }
}

int meadowlark::load_images() {
   std::vector<std::string> fileNames;
   for (auto& entry : fs::directory_iterator(stPath)) {
      std::string revSlash = entry.path().string();
      std::replace(revSlash.begin(), revSlash.end(), '\\', '/');
      fileNames.push_back(revSlash);
      //std::cout << entry.path() << '\n';
      //printf("%s\n", entry.path());
   }

   for (int i = 0; i < fileNames.size(); i++) {
      std::cout << "File " << i << ": " << i << fileNames[i].c_str() << std::endl;
   }

   buffer_size = ImgSize * fileNames.size(); // total size of all images to load.
   im_buffer = (uint8_t*)malloc(buffer_size);
   im_ptr = im_buffer;
   //printf("_msize(im_buffer): %u\n", _msize(im_buffer));

   std::cout << "buffer_size: " << (unsigned int)buffer_size << std::endl;
   std::cout << "buffer_size/ImgSize: " << (unsigned int)(buffer_size / ImgSize) << std::endl;

   for (int i = 0; i < fileNames.size(); i++) {

      PBITMAPFILEHEADER file_header;
      PBITMAPINFOHEADER info_header;

      //printf("sizeof(PBITMAPFILEHEADER): %llu\n", sizeof(tagBITMAPFILEHEADER));
      //printf("sizeof(PBITMAPINFOHEADER): %llu\n", sizeof(tagBITMAPINFOHEADER));

      std::ifstream file(fileNames[i].c_str());


      if (file) {

         //printf("fileNames[%d].c_str(): %s\n", i, fileNames[i].c_str());
         file.seekg(0, std::ios::end);
         std::streampos length = file.tellg();
         file.seekg(0, std::ios::beg);
         //printf("length: %d\n", (int)length);


         uint8_t* file_buff = (uint8_t*)malloc((int)length);
         HANDLE fileH = INVALID_HANDLE_VALUE;

         std::wstring stemp = std::wstring(fileNames[i].begin(), fileNames[i].end());
         fileH = CreateFile(stemp.c_str(),
            GENERIC_READ,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

         DWORD dwBytesRead = 0;
         BOOL bErrorFlag = FALSE;

         if (fileH == INVALID_HANDLE_VALUE)
         {
            std::cout << "Awe Shucks Failed to open file" << std::endl;
            return 1;
         }

         bErrorFlag = ReadFile(
            fileH,           // open file handle
            file_buff,      // start of data to write
            (int)length,  // number of bytes to write
            &dwBytesRead, // number of bytes that were written
            NULL);            // no overlapped structure

         if (FALSE == bErrorFlag)
         {
            //DisplayError(TEXT("WriteFile"));
            std::cout << "Terminal failure: Unable to read from  file." << std::endl;
            return 2;
         }
         CloseHandle(fileH);

         file_header = (PBITMAPFILEHEADER)(file_buff);
         info_header = (PBITMAPINFOHEADER)(file_buff + sizeof(BITMAPFILEHEADER));

         //printf("bfSize: %d dim product: %u\n", file_header->bfSize, ImgSize);
         //printf("bfOffBits: %d\n", file_header->bfOffBits);
         memcpy((void*)(im_buffer + ImgSize * i), (void*)(file_buff + file_header->bfOffBits), ImgSize);
         free(file_buff);
         file.close();
      }
      else {
         std::cout << "Failed to open: " << fileNames[i] << std::endl;
         return 1;
      }
   }
   return 0;
}

int meadowlark::slm_thread(void* data)
{
   SLM_THREAD_DATA* uiData = (SLM_THREAD_DATA*)data;

   // Construct a Blink_SDK instance

   //if bits per pixel is wrong, the lower level code will figure out
   //what it should be and construct properly.
   Create_SDK(bits_per_pixel, &n_boards_found, &constructed_okay, is_nematic_type, RAM_write_enable, use_GPU_if_available, 10U, 0);

   // return of 1 means okay
   if (constructed_okay != 1) {
      std::string temp_s = Get_last_error_message();
      std::wstring stemp = std::wstring(temp_s.begin(), temp_s.end());
      std::cout << stemp.c_str() << std::endl;
      uiData->_msg_label->set_text("!!!SLM NOT FOUND!!!");
      uiData->_slm_button->value(0);
      uiData->_view->refresh();
      //::AfxMessageBox(stemp.c_str());
      return 1;
      
   }

   // return of 0 means okay, return -1 means error
   if (n_boards_found > 0)
   {
      board_number = 1;
      height = Get_image_height(board_number);
      width = Get_image_width(board_number);
      depth = Get_image_depth(board_number); //bits per pixel
      Bytes = depth / 8;
      ImgSize = height * width * Bytes;
   }
   else {
      //return true;
   }
   get_paths();
   load_lut();
   load_images();
   //return false;
   //to keep the example generic a blank wavefront correction is used, you can replace this with your real wavefront correction
   unsigned char* WFC = new unsigned char[ImgSize];
   memset(WFC, 0, ImgSize);

   // Create two vectors to hold values for two SLM images with opposite ramps.
   unsigned char* ImageOne = new unsigned char[ImgSize];
   //unsigned char* ImageTwo = new unsigned char[ImgSize];
   memset(ImageOne, 0, ImgSize);
   //memset(ImageTwo, 0, ImgSize);

   keep_alive = true;
   while (keep_alive) {
      uiData->_msg_label->set_text("!!!SLM ARMED!!!");
      uiData->_view->refresh();
      // Wait for signal to start
      std::unique_lock slp(*uiData->_slm_mutex);
      uiData->signal_slm->wait(slp);

      uiData->_msg_label->set_text("!!!SLM RUNNING!!!");
      uiData->_view->refresh();


      //start the SLM with a blank image
      Write_image(board_number, ImageOne, ImgSize, false, FlipImmediate, OutputPulseImageFlip, OutputPulseImageRefresh, 5000);
      std::cout << "Just wrote all black aka 0deg phase shift to SLM" << std::endl;
      ImageWriteComplete(board_number, 5000);
      //printf("After ImageWriteComplete 5000\n");



      //start the SLM with first image
      Write_image(board_number, (unsigned char*)im_buffer, ImgSize, false, FlipImmediate, OutputPulseImageFlip, OutputPulseImageRefresh, 5000);
      std::wcout << "Just wrote first Pattern to SLM" << std::endl;
      ImageWriteComplete(board_number, 5000);

      char hold_nLine = 0;

      std::cout << std::endl << std::endl << "First Image Once started you will have 5 sec timeout" << std::endl;

      std::cout << "\n\r\n\r****Live Running****" << std::endl;


      // Generate phase gradients
      //int VortexCharge = 5;
      //bool RGB = false;
      //bool fork = false;
      //Generate_LG(ImageOne, WFC, width, height, depth, VortexCharge, width / 2.0, height / 2.0, fork, RGB);
      //VortexCharge = 3;
      //Generate_LG(ImageTwo, WFC, width, height, depth, VortexCharge, width / 2.0, height / 2.0, fork, RGB);


      is_running = true;
      offset = ImgSize;

      //printf("Waiting for first trigger, will timeout in aprox 5000ms\n");

      while (is_running && keep_alive) {
         //write image returns on DMA complete, ImageWriteComplete returns when the hardware
         //image buffer is ready to receive the next image. Breaking this into two functions is 
         //useful for external triggers. It is safe to apply a trigger when Write_image is complete
         //and it is safe to write a new image when ImageWriteComplete returns

         //printf("Loading pattern sequence number: %u\n", offset/ImgSize + 1);
         int write_value = Write_image(board_number, ((unsigned char*)im_buffer + offset), ImgSize, ExternalTrigger, FlipImmediate, OutputPulseImageFlip, OutputPulseImageRefresh, 5000);
         int write_complete = ImageWriteComplete(board_number, 5000);
         offset = (offset + ImgSize) % buffer_size;
         if (write_complete < 0) {
            is_running = false;
            std::cout << "Trigger timed out" << std::endl;
         }

      }
      //CLEAR the SLM with a blank image
      Write_image(board_number, ImageOne, ImgSize, false, FlipImmediate, OutputPulseImageFlip, OutputPulseImageRefresh, 5000);
      std::cout << "Just wrote all black aka 0deg phase shift to SLM" << std::endl;
      ImageWriteComplete(board_number, 5000);
      Sleep(250);
      std::cout << "\n\r\n\r****Setting All SLM Values to 0****\n\r" << std::endl;
   }
      std::cout << "All Done" << std::endl;
      free(im_buffer);

      //CLEAR the SLM with a blank image
      Write_image(board_number, ImageOne, ImgSize, false, FlipImmediate, OutputPulseImageFlip, OutputPulseImageRefresh, 5000);
      std::cout << "Just wrote all black aka 0deg phase shift to SLM" << std::endl;
      ImageWriteComplete(board_number, 5000);
      Sleep(250);
      std::cout << "\n\r\n\r****Setting All SLM Values to 0****\n\r" << std::endl;


   delete[]ImageOne;
   //delete[]ImageTwo;
   delete[]WFC;
   disable_meadowlark();
   uiData->_msg_label->set_text("!!!SLM Disconnected!!!");
   uiData->_slm_button->value(0);
   uiData->_view->refresh();
   return 0;
}

void meadowlark::disable_meadowlark()
{
      SLM_power(false);
      Delete_SDK();
 }

