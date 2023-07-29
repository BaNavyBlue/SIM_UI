/* This is where I define my function pointer for the USB thread */

#include "USB_INTERFACE.h"

void USB_INTERFACE::startUSB()
{
    /* USB Thread lambda function*/
    auto USB_THREAD = [this]() {
        std::cout << "USB Thread Started." << std::endl;
        this->dev = NULL; /* the device handle */

        int ret;
        void* async_read_context = NULL;
        void* async_write_context = NULL;

        int device_ready = 0;
        int result = 0;

        std::cout << "Before libusb_init(NULL)" << std::endl;
        libusb_init(NULL); /* initialize the library */
        std::cout << "Before open device" << std::endl;
        this->dev = libusb_open_device_with_vid_pid(NULL, MY_VID, MY_PID);
        std::cout << "Before Claim Device" << std::endl;

        if (this->dev != NULL)
        {
            libusb_detach_kernel_driver(this->dev, 0);
            result = libusb_claim_interface(this->dev, 0);
            if (result != 0)
            {
                device_ready = 1;
            }
        }
        else
        {
            std::cout << "Failed to connect to USB DEVICE" << std::endl;
            return -1;
        }
        std::cout << "success: device " << MY_VID << ":" << MY_PID << " opened" << std::endl;
        this->thd_dat->usb_running = true;


        uint8_t send_data = 1;
        uint8_t running = 1;

        thd_dat->outgoing.flags = 0;
        thd_dat->incoming.flags = 0;
        thd_dat->outgoing.mode = 0;
        thd_dat->outgoing.count = 1;
        thd_dat->outgoing.mode |= THREE_BEAM;
        thd_dat->outgoing.mode |= COUNT_MODE;
        this->thd_dat->outgoing.flags |= STOP_CAPTURE | CHANGE_FPS | SET_RUN_MODE | SET_SIM_MODE;
        this->thd_dat->outgoing.fps = 5.0;
        /*****************************************************************************************/
        while (this->thd_dat->usb_running) {

            //std::unique_lock<std::mutex> flg(this->thd_dat->usb_crit);
            if (this->thd_dat->outgoing.flags & (START_CAPTURE|STAGE_MOVE_COMPLETE|SET_RUN_MODE|CHANGE_Z_STEPS|STOP_CAPTURE|CHANGE_FPS|SET_EXPOSURE|TOGGLE_BLANKING|SET_LASER_MODE|SET_SIM_MODE)) {
                int actualSize;
                std::cout << "before bulk transfer" << std::endl;
                int ret = libusb_bulk_transfer(dev, EP_OUT, (unsigned char*)&thd_dat->outgoing, sizeof(usb_data), &actualSize, 0);
               
                //#endif
                if (ret < 0)
                {
                    std::cout << "error writing:\r\n" << std::endl;
                }
                else
                {
                    std::cout << "success: bulk write " << actualSize << " bytes" << std::endl;

             
                    thd_dat->outgoing.flags &= ~(START_CAPTURE | STAGE_MOVE_COMPLETE | SET_RUN_MODE | CHANGE_Z_STEPS | STOP_CAPTURE | CHANGE_FPS|SET_EXPOSURE|TOGGLE_BLANKING|SET_LASER_MODE | SET_SIM_MODE); //~(CHANGE_FPS);
                    //thd_data->incoming->flags &= ~CHANGE_CONFIG;
                   
                }
            }
            //flg.unlock();


            //flg.lock();
            int actualLength = 0;
            ret = libusb_bulk_transfer(this->dev, EP_IN | 0x80, (unsigned char*) & this->thd_dat->incoming, sizeof(usb_data), &actualLength,0);
            //std::cout << "What?" << std::endl;
            //flg.unlock();

            if (ret < 0)
            {
                //asio::placeholders::error();
                std::cout << "usb return failure" << std::endl;
            }
            if(this->thd_dat->incoming.flags & SEND_TRIGG){
                this->thd_dat->signal_THOR.notify_one();
                std::cout << "Trigger command recieved" << std::endl;
                this->thd_dat->incoming.flags &= ~SEND_TRIGG;
            }
            if (this->thd_dat->incoming.flags & STOP_Z_STACK) {
                std::cout << "Stop Z Stack received" << std::endl;
                this->thd_dat->incoming.flags &= ~STOP_Z_STACK;
                this->thd_dat->outgoing.flags |= STOP_CAPTURE;
                this->thd_dat->trigger_running = false;
                //this->thd_dat->start_stop_butt_ptr->reset();
            }

            if (this->thd_dat->incoming.flags & STOP_COUNT) {
                std::cout << "Stop Count received" << std::endl;
                this->thd_dat->incoming.flags &= ~STOP_COUNT;
                this->thd_dat->outgoing.flags |= STOP_CAPTURE;
                this->thd_dat->trigger_running = false;
                //this->thd_dat->start_stop_butt_ptr->reset();
            }


            if (this->thd_dat->incoming.flags & SET_EXPOSURE) {
                this->thd_dat->expVal = this->thd_dat->incoming.exposure;
                this->thd_dat->exp.second.get()->set_text(std::to_string(this->thd_dat->expVal));
                this->thd_dat->incoming.flags &= ~SET_EXPOSURE;
            }
            if (this->thd_dat->incoming.flags & CHANGE_FPS) {
                std::cout << "set fps: " << this->thd_dat->incoming.fps << std::endl;
                this->thd_dat->fpsVal = this->thd_dat->incoming.fps;
                this->thd_dat->fps.second.get()->set_text(std::to_string(this->thd_dat->incoming.fps));
                this->thd_dat->incoming.flags &= ~CHANGE_FPS;
            }
            //flg.unlock();
            //if (!(this->thd_dat->incoming.count % 100)) {
            //    std::cout << "fps: " << this->thd_dat->incoming.fps << std::endl;
            //    std::cout << "exposure time: " << this->thd_dat->incoming.exposure << std::endl;
            //}
            /*else if (actualLength > 0)
            {
                std::cout << "got: " << actualLength << " bytes" << std::endl;
            }*/

        }
       std::cout << "Exiting USB LOOP." << std::endl;
#ifdef TEST_CLAIM_INTERFACE
        // Finished using the device.
        libusb_release_interface(this->dev, 0);
#endif

        if (this->dev)
        {
          libusb_exit(NULL)  ;
        }
        std::cout << "Done." << std::endl;

        return 0;
    };
    USB_thread_ptr = new std::thread(USB_THREAD);
}

void USB_INTERFACE::stopUSB()
{
    thd_dat->usb_running = false;
    if (USB_thread_ptr->joinable()) {
        USB_thread_ptr->join();
        std::cout << "Joining USB Thread" << std::endl;
    }
}

void USB_INTERFACE::sendData()
{
    std::unique_lock<std::mutex> flg(this->thd_dat->usb_crit);
    int actualSize;
    int ret = libusb_bulk_transfer(dev, EP_OUT, (unsigned char*)&thd_dat->outgoing, sizeof(usb_data), &actualSize,0);
    flg.unlock();
    //#endif
    if (ret < 0)
    {
        std::cout << "error writing:\r\n" <<  std::endl;
    }
    else
    {
        std::cout << "success: bulk write " << actualSize << " bytes" << std::endl;

        flg.lock();
        thd_dat->outgoing.flags &= 0;//~(CHANGE_CONFIG | LAPSE_STOP | START_COUNT | STOP_COUNT | CAMERAS_ACQUIRED | RELEASE_CAMERAS | START_CAPTURE | START_LIVE | START_Z_STACK | TOGGLE_DIG_MOD | TOGGLE_EMMISION); //~(CHANGE_FPS);
        //thd_data->incoming->flags &= ~CHANGE_CONFIG;
        flg.unlock();
    }
}

int USB_INTERFACE::calc_num_images()
{
    int image_count = 1;
    if (thd_dat->count_run_state) {
        image_count = (thd_dat->SIM_angles * thd_dat->SIM_phases * thd_dat->count)*thd_dat->lapse_counts;
        return image_count;
    }
    else if (thd_dat->triggerStage) {
        image_count = (thd_dat->SIM_angles * thd_dat->SIM_phases * thd_dat->positions)*thd_dat->lapse_counts;
        return image_count;
    }
    return image_count;
}

void USB_INTERFACE::change_lapse_count() {
    if (thd_dat->run_time_lapse) {
        thd_dat->lapse_counts = std::ceil(thd_dat->lapse_time * 60 / thd_dat->lapseVal);
    }
    else {
        thd_dat->lapse_counts = 1;
    }
    thd_dat->_config_label.get()->set_text(conf_string());
}

std::string USB_INTERFACE::conf_string()
{
    std::string state = "Mode: Free Run, ";
    std::string frames = "Total Frames: " + std::to_string(calc_num_images()) + ", ";
    std::string lapse_captures = "Lapse counts: " + std::to_string(thd_dat->lapse_counts);
    if (thd_dat->count_run_state) {
        state = "Mode: Frame Count, ";
    }
    else if (thd_dat->triggerStage) {
        state = "Mode: Z-Step, ";
    }
    return state + frames + lapse_captures;
}

