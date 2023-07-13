/*
 The USB Thrad Header
 Most of this code is form the LIBUSB example code modified for our needs
 I put all of the LIBUSB headers and libraries in this projects directory.
 Under Linker Input I needed to specify the path of the .lib file
 C:\Users\Callisto\Documents\abajor\M25_basler\basler_candidate\libusb-win32-bin-1.2.6.0\libusb-win32-bin-1.2.6.0\lib\msvc_x64\libusb.lib
 use https://sourceforge.net/projects/libusbk/ to download driver utility manually select the unprogrammed cypress usb driver.
*/

#pragma once

#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H
#include "SIM_UI_Headers.hpp"
//#include "lusb0_usb.h"
#include "libusb.h"



//////////////////////////////////////////////////////////////////////////////
// TEST SETUP (User configurable)

// Issues a Set configuration request
#define TEST_SET_CONFIGURATION

// Issues a claim interface request
#define TEST_CLAIM_INTERFACE

// Use the libusb-win32 async transfer functions. see
// transfer_bulk_async() below.
//#define TEST_ASYNC

// Attempts one bulk read.
#define TEST_BULK_READ

// Attempts one bulk write.
#define TEST_BULK_WRITE

//////////////////////////////////////////////////////////////////////////////
// DEVICE SETUP (User configurable)

// Device vendor and product id.
#define MY_VID 0x04b4 // Cypress Psoc 5lp default
#define MY_PID 0x8051

// Device configuration and interface id.
#define MY_CONFIG 1
#define MY_INTF 0

// Device endpoint(s)
#define EP_IN 0x01
#define EP_OUT 0x02

// Device of bytes to transfer.
const unsigned BUF_SIZE = sizeof(usb_data);

//////////////////////////////////////////////////////////////////////////////

class USB_INTERFACE {
public:
	void startUSB();
	void stopUSB();
	void sendData();
	USB_THREAD_DATA* thd_dat;
	//libusb_device_handle* open_dev(void);
	libusb_device_handle* dev;
private:
	std::thread* USB_thread_ptr;
};


//usb_dev_handle* open_dev(void);
//void* USB_THREAD(void* data);

#endif
