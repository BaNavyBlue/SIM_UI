cmake_minimum_required(VERSION 3.9.6...3.15.0)
project(Tabs LANGUAGES C CXX)

if (NOT ELEMENTS_ROOT)
   message(FATAL_ERROR "ELEMENTS_ROOT is not set")
endif()

# Make sure ELEMENTS_ROOT is an absolute path to add to the CMake module path
get_filename_component(ELEMENTS_ROOT "${ELEMENTS_ROOT}" ABSOLUTE)
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${ELEMENTS_ROOT}/cmake")

# If we are building outside the project, you need to set ELEMENTS_ROOT:
if (NOT ELEMENTS_BUILD_EXAMPLES)
   include(ElementsConfigCommon)
   set(ELEMENTS_BUILD_EXAMPLES OFF)
   add_subdirectory(${ELEMENTS_ROOT} elements)
endif()

set(ELEMENTS_APP_PROJECT "SIM_UI")
set(ELEMENTS_APP_TITLE "SIM_UI")
set(ELEMENTS_APP_COPYRIGHT "Copyright (c) 2016-2020 Joel de Guzman")
set(ELEMENTS_APP_ID "com.cycfi.notebook")
set(ELEMENTS_APP_VERSION "1.0")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/includes; C:/Users/Titan/source/repos/vcpkg/installed/x86-windows/include)

add_definitions(-D_AFXDLL)
set(CMAKE_MFC_FLAG 2)
#add_executable(CMakeSetup WIN32 ${SRCS})

#target_link_libraries(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/Blink_C_wrapper.lib)
#target_link_libraries(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/ImageGen.lib)
find_library(KERN32 kernel32.lib)
find_library(USER32 user32.lib)
find_library(GDI32 gdi32.lib)
find_library(WINSPOOL winspool.lib)
find_library(COMLG32 comdlg32.lib)
find_library(ADVAPI32 advapi32.lib)
find_library(SHELL32 shell32.lib)
find_library(OLE32 ole32.lib)
find_library(OLEAUT32 oleaut32.lib)
find_library(UUID uuid.lib)
find_library(ODBC32 odbc32.lib)
find_library(ODBCCP32 odbccp32.lib)
#target_link_libraries(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv_world455.lib)
#find_package(libusb CONFIG REQUIRED)
#target_include_directories(main PRIVATE ${LIBUSB_INCLUDE_DIRS})
#target_link_libraries(main PRIVATE ${LIBUSB_LIBRARIES})
#link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
#add_library(Blink_C_Wrapper ${CMAKE_CURRENT_SOURCE_DIR}/lib/Blink_C_Wrapper.lib)
#add_library(ImageGen ${CMAKE_CURRENT_SOURCE_DIR}/lib/ImageGen.lib)
#add_library(opencv_world455 ${CMAKE_CURRENT_SOURCE_DIR}/lib/opencv_world455.lib)

set(ELEMENTS_APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp; ${CMAKE_CURRENT_SOURCE_DIR}/src/SIM_UI.cpp; ${CMAKE_CURRENT_SOURCE_DIR}/src/SLM.cpp ; ${CMAKE_CURRENT_SOURCE_DIR}/src/meadowlark_slm.cpp; ${CMAKE_CURRENT_SOURCE_DIR}/src/Stage_Control.cpp; ${CMAKE_CURRENT_SOURCE_DIR}/src/PI_STAGE.cpp; ${CMAKE_CURRENT_SOURCE_DIR}/src/USB_INTERFACE.cpp; ${CMAKE_CURRENT_SOURCE_DIR}/src/THOR_STAGE.cpp)

set(ELEMENTS_APP_DEPENDENCIES  ${CMAKE_CURRENT_SOURCE_DIR}/lib/libusb-1.0.lib; ${CMAKE_CURRENT_SOURCE_DIR}/lib/Blink_C_wrapper.lib; ${CMAKE_CURRENT_SOURCE_DIR}/lib/PI_GCS2_DLL_x64.lib; ${CMAKE_CURRENT_SOURCE_DIR}/lib/Thorlabs.MotionControl.KCube.DCServo.lib)




# For your custom application icon on macOS or Windows see cmake/AppIcon.cmake module
include(AppIcon)
include(ElementsConfigApp)

