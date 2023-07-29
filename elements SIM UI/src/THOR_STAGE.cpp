#include <THOR_STAGE.h>

int THOR_STAGE::THOR_STAGE_THD(void* data_ptr)
{
    
    data = (STAGE_THREAD_DATA*)data_ptr;


    // optionally set a position in device units.
    //int position = 34555 * 2;

    // for the Z825B actuator 512counts/rev * 67.49rev = 34555 counts

    //int revolution = 512;
    //double one_mm = 67.49;

    double target_velocity = 2.3; // mm/s
    double target_acceleration = 1.5; // mm/s^2

    // Optionally change this value to a desired velocity (in device units/second)--*--+cx vc                                                                       
    //int velocity = std::ceil(_revolution * target_velocity * _one_mm);
    //int acceleration = std::ceil(revolution*target_acceleration*one_mm);
    //std::cout << "calculated velocity counts: " << velocity << std::endl;
    //std::cout << "calculated acceleration counts: " << acceleration << std::endl;

   // identify and access device
    
    sprintf_s(testSerialNo, "%d", serialNo);
    std::cout << "Before Build List" << std::endl;
    // Build list of connected device
    if (TLI_BuildDeviceList() == 0)
    {
        // get device list size 
        short n = TLI_GetDeviceListSize();
        // get KDC serial numbers
        char serialNos[100];
        TLI_GetDeviceListByTypeExt(serialNos, 100, 27);

        // Search serial numbers for given serial number
        if (strstr(serialNos, testSerialNo)) {
            std::cout << "Device " << testSerialNo << " found" << std::endl;
            threadIsAlive = true;
        }
        else {
            std::cout << "Device " << testSerialNo << " Not Found" << std::endl;
            return -1;
        }

        // open device
        if (CC_Open(testSerialNo) == 0)
        {
            // start the device polling at 200ms intervals
            CC_StartPolling(testSerialNo, 1);

            unsigned int homing_velocity = CC_GetHomingVelocity(testSerialNo);
            std::cout << "homing velocity: " << homing_velocity << std::endl;

            CC_SetHomingVelocity(testSerialNo, homingVel);

            homing_velocity = CC_GetHomingVelocity(testSerialNo);
            std::cout << "new homing velocity: " << homing_velocity << std::endl;

            /* Atempting to prevent stage from homing to fully extended position */
            MOT_HomingParameters homeParam;
            homeParam.direction = MOT_Backwards;
            homeParam.limitSwitch = MOT_ReverseLimitSwitch;
            homeParam.offsetDistance = 10;
            homeParam.velocity = homingVel;

            CC_SetHomingParamsBlock(testSerialNo, &homeParam);
            Sleep(100);
            // Home device
            CC_ClearMessageQueue(testSerialNo);
            CC_Home(testSerialNo);
            std::cout << "Device " << testSerialNo << " homing" << std::endl;
            waitForHome();
            // wait for completion
            //WORD messageType;
            //WORD messageId;
            //DWORD messageData;
            ////int curpos = CC_GetPosition(testSerialNo);
            ////std::cout << "curpos: " << curpos << std::endl;
            //bool message = CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
            //std::cout << "bool: " << message << " messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
            //while (messageType != 2 || messageId != 0)
            //{
            //    message = CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
            //    //std::cout << "bool: " << message << " messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
            //}
            std::cout << "Escaped while finished homing" << std::endl;

            int currentVelocity, currentAcceleration;
            CC_GetVelParams(testSerialNo, &currentAcceleration, &currentVelocity);
            std::cout << "currentAcceleration: " << currentAcceleration << " currentVelocity: " << currentVelocity << std::endl;
            CC_SetVelParams(testSerialNo, acceleration, velocity);
            std::cout << "new Acceleration: " << acceleration << " new Velocity: " << velocity << std::endl;


            while (threadIsAlive) {
                std::unique_lock crit(command_protect);
                position_counts = CC_GetPosition(testSerialNo);
                crit.unlock();
                position_mm = (double)position_counts / one_mm_in_counts;
                data->_pos_label->set_text(std::to_string(position_mm));
                data->_view->refresh();
                Sleep(200);
            }
            //std::cout << "curpos: " << curpos << std::endl;

            //int currentVelocity, currentAcceleration;
            //CC_GetVelParams(testSerialNo, &currentAcceleration, &currentVelocity);
            //std::cout << "currentAcceleration: " << currentAcceleration << " currentVelocity: " << currentVelocity << std::endl;
            //// set velocity if desired
            ////if(velocity > 0)
            ////{
            ////    int currentVelocity, currentAcceleration;
            ////    CC_GetVelParams(testSerialNo, &currentAcceleration, &currentVelocity);
            ////    std::cout << "currentAcceleration: " << currentAcceleration << " currentVelocity: " << currentVelocity << std::endl;
            ////    CC_SetVelParams(testSerialNo, currentAcceleration, velocity);
            ////}

            //// move to position (channel 1)
            //CC_ClearMessageQueue(testSerialNo);
            //std::cout << "Clearing Message Que" << std::endl;
            //CC_MoveToPosition(testSerialNo, position);
            //std::cout << "Moving To Position" << std::endl;
            //CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
            //std::cout << "messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
            //while (messageType != 2 || messageId != 1)
            //{
            //    CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
            //    //std::cout << "messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
            //}
            //CC_ClearMessageQueue(testSerialNo);
            //curpos = CC_GetPosition(testSerialNo);
            //std::cout << "curpos: " << curpos << std::endl;
            CC_ClearMessageQueue(testSerialNo);
            // stop polling
            CC_StopPolling(testSerialNo);
            std::cout << "Stop Polling" << std::endl;
            // close device
            CC_Close(testSerialNo);
            std::cout << "Closing Device" << std::endl;
        }
    }

    // Uncomment this line if you are using simulations
    //TLI_UninitializeSimulations();

    //char c = _getch();
    //std::cout << "After _getch()" << std::endl;
    return 0;
}

bool THOR_STAGE::waitForMove()
{
    // wait for completion
    bool waiting = true;
    WORD messageType;
    WORD messageId;
    DWORD messageData;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::unique_lock crit(command_protect);
    CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
    crit.unlock();
    //std::cout << "messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
    while (waiting)
    {
        crit.lock();
        CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
        crit.unlock();
        //std::cout << "messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
        if (messageType == 2 && (messageId == 1 || messageId == 2)) {
            waiting = false;
            crit.lock();
            CC_ClearMessageQueue(testSerialNo);
            crit.unlock();
            if (messageId == 2) {
                crit.lock();
                CC_Home(testSerialNo);
                crit.unlock();
                std::cout << "re homing" << std::endl;
                waitForHome();
            }
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Move time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[us]" << std::endl;
    return true;
}

bool THOR_STAGE::waitForHome()
{
    // wait for completion
    WORD messageType;
    WORD messageId;
    DWORD messageData;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    std::unique_lock crit(command_protect);
    CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
    crit.unlock();
    //std::cout << "messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
    while (messageType != 2 || messageId != 0)
    {
        crit.lock();
        CC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
        crit.unlock();
        //std::cout << "messageType: " << messageType << " messageId: " << messageId << " messageData: " << messageData << std::endl;
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Move Home time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[us]" << std::endl;
    return true;
}

void THOR_STAGE::moveRel(double displace)
{
    double new_position = position_mm + displace;
    int moveTo = std::round(new_position * one_mm_in_counts);
    if (new_position < minPos_mm) {
        moveTo = minPos_counts;
        std::cout << "stage SN: " << testSerialNo << " move Home" << std::endl;
        std::unique_lock crit(command_protect);
        //CC_ClearMessageQueue(testSerialNo);
        CC_Home(testSerialNo);
        crit.unlock();
        waitForHome();
    }
    else if (new_position > maxPos_mm) {
        moveTo = maxPos_counts;
        std::cout << "stage SN: " << testSerialNo << " moveTo: " << moveTo << std::endl;
        std::unique_lock crit(command_protect);
        //CC_ClearMessageQueue(testSerialNo);
        CC_MoveToPosition(testSerialNo, moveTo);
        crit.unlock();
        waitForMove();
    }
    else {
        std::cout << "stage SN: " << testSerialNo << " moveTo: " << moveTo << std::endl;
        std::unique_lock crit(command_protect);
        //CC_ClearMessageQueue(testSerialNo);
        CC_MoveToPosition(testSerialNo, moveTo);
        crit.unlock();
        waitForMove();
    }
}

void THOR_STAGE::moveAbs(double displace)
{
    int moveTo = std::round(displace * one_mm_in_counts);
    if (displace < minPos_mm) {
        moveTo = minPos_counts;
        std::cout << "stage SN: " << testSerialNo << " move Home" << std::endl;
        std::unique_lock crit(command_protect);
        //CC_ClearMessageQueue(testSerialNo);
        CC_Home(testSerialNo);
        crit.unlock();
        waitForHome();
    }
    else if (displace > maxPos_mm) {
        moveTo = maxPos_counts;
        std::cout << "stage SN: " << testSerialNo << " moveTo: " << moveTo << std::endl;
        std::unique_lock crit(command_protect);
        //CC_ClearMessageQueue(testSerialNo);
        CC_MoveToPosition(testSerialNo, moveTo);
        crit.unlock();
        waitForMove();
    }
    else {
        std::cout << "stage SN: " << testSerialNo << " moveTo: " << moveTo << std::endl;
        std::unique_lock crit(command_protect);
        //CC_ClearMessageQueue(testSerialNo);
        CC_MoveToPosition(testSerialNo, moveTo);
        crit.unlock();
        waitForMove();
    }
}

void THOR_STAGE::runSevenPhaseSeq()
{
    if (stage_seq_thd_ptr != nullptr && stage_seq_thd_ptr->joinable() && !axial_stage_seq_run) {
        stage_seq_thd_ptr->join();
    }
    auto seq_thread = [this]()
    {
        //this->data->returnPos = this->position;
        //std::cout << "Pos: " << 0 << " ";
        //this->moveRel(this->data->startDisplace);
        //std::cout << "Stage Moved To Start Position" << std::endl;

        //uint32_t wait_final_frame = std::ceil(1 / this->data->usb_dat->fpsVal * this->data->usb_dat->min_frames * 1000);
        //std::cout << "Wait Final Frame: " << (int)wait_final_frame << "ms" << std::endl;
        //signal_slm->notify_one(); // Make SURE SLM is waiting for trigger
        //this->data->usb_dat->outgoing.flags |= START_CAPTURE;


        int i = 0;
        while (i < 6) {
            double new_position = blue_axial_shift * (i + 1);
            std::unique_lock slp(this->data->sleep_thread);
            this->data->signal_stage->wait(slp);
            this->moveAbs(new_position);
            this->data->usb_dat->outgoing.flags |= STAGE_MOVE_COMPLETE;
            std::cout << "moved pos:" << i + 1 << std::endl;
            ++i;
        }
        CC_Home(this->testSerialNo);
        this->waitForHome();
        axial_stage_seq_run = false;
        std::cout << "Axial Phase Shift finished, returned home" << std::endl;
    };
    //if (trigger_thread_ptr = nullptr) {
    axial_stage_seq_run = true;
    stage_seq_thd_ptr = new std::thread(seq_thread);
    std::cout << "Axial Phase thread started" << std::endl;
}