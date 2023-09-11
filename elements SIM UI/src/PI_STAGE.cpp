#include "PI_STAGE.h"


int PI_STAGE::PIStage_Thread(void* data_ptr) {

    
    data = (STAGE_THREAD_DATA*) data_ptr;

    OpenConnection(USB,"");

    if (iD < 0)
    {
        std::cout << "Unable to connect.";
        return 1;
    }

    try
    {
        threadIsAlive = true;
        PrintControllerIdentification(iD);

        sAxis = "1";

        SetServoState(iD, sAxis, SERVO_ON);

        minPos = GetMinPositionLimit(iD, sAxis);
        maxPos = GetMaxPositionLimit(iD, sAxis);

        std::cout << "Successfully determined min (" << minPos << ") and max (" << maxPos << ") position limits." << std::endl;

        int nInChan;
        int nOutChan;
        if (PI_qTIO(iD, &nInChan, &nOutChan)) std::cout << "num in chan: " << nInChan << " num out chan: " << nOutChan << std::endl;

        range = maxPos - minPos;
        //queryPos();
        while (threadIsAlive) {
            queryPos();
            Sleep(400);
        }
        //double targetPos = minPosLimit + (range * 0.1);

        // do absolute motions
        /*if (!PI_MOV(iD, sAxis.c_str(), &targetPos))
        {
            throw std::runtime_error("Unable to approach minimum position limit.");
        }*/

        //WaitForMotionDone(iD, sAxis.c_str());
        //std::cout << "Successfully approached minimum position limit, to " << targetPos << std::endl;

        //targetPos = maxPosLimit - (range * 0.1);

        //if (!PI_MOV(iD, sAxis.c_str(), &targetPos))
        //{
        //    throw std::runtime_error("Unable to approach maximum position limit.");
        //}

        //WaitForMotionDone(iD, sAxis.c_str());
        //std::cout << "Successfully approached maximum position limit, to " << targetPos << std::endl;

        //targetPos = maxPosLimit * 0.5 * -1;

        //// do relative motions
        //if (!PI_MVR(iD, sAxis.c_str(), &targetPos))
        //{
        //    throw std::runtime_error("Unable to do relative move backward.");
        //}

        //WaitForMotionDone(iD, sAxis.c_str());
        //std::cout << "Successfully executed relative move backward by " << targetPos << " units" << std::endl;

        //targetPos = targetPos * -1;

        //if (!PI_MVR(iD, sAxis.c_str(), &targetPos))
        //{
        //    throw new std::runtime_error("Unable to do relative move forward.");
        //}

        //WaitForMotionDone(iD, sAxis.c_str());
        //std::cout << "Successfully executed relative move forward by " << targetPos << " units" << std::endl;

        //SetServoState(iD, sAxis.c_str(), SERVO_OFF);
        PI_CloseConnection(iD);
        std::cout << "Closing connection." << std::endl;
    }
    catch (std::runtime_error e)
    {
        CloseConnectionWithComment(iD, e.what());
        return 1;
    }

    return 0;

}

void PI_STAGE::CloseConnectionWithComment(int iD, std::string comment)
{
    std::cout << comment << std::endl;

    ReportError(iD);
    PI_CloseConnection(iD);
}

double PI_STAGE::GetMinPositionLimit(int ID, std::string axis)
{
    double minPosLimit = 0;

    if (!PI_qTMN(ID, axis.c_str(), &minPosLimit))
    {
        throw std::runtime_error("Unable to get minimum position limit.");
    }

    return minPosLimit;
}

double PI_STAGE::GetMaxPositionLimit(int ID, std::string axis)
{
    double maxPosLimit;

    if (!PI_qTMX(ID, axis.c_str(), &maxPosLimit))
    {
        throw std::runtime_error("Unable to get maximum position limit.");
    }

    return maxPosLimit;
}

void PI_STAGE::OpenConnection(ConnectionType type, std::string hostName)
{
    iD = 0;
    std::string connType;

    switch (type)
    {
    case Dialog:
        iD = PI_InterfaceSetupDlg("");
        connType = "Dialog";
        break;
    case RS232:
        iD = PI_ConnectRS232(1, 115200);
        connType = "RS232";
        break;
    case TCPIP:
        iD = PI_ConnectTCPIP(hostName.c_str(), 50000);
        connType = "TCPIP";
        break;
    case USB:
        char connectedUsbController[1024];
        int noDevices = PI_EnumerateUSB(connectedUsbController, 1024, "");
        std::cout << "Found " << noDevices << " controllers connected via USB, connecting to the first one:" << std::endl << connectedUsbController << std::endl;
        std::string PI_stage = "E-754.1CD Physik Instrumente (PI) SN 118050564";
 
        //char* pos = strchr(connectedUsbController, '\n');

        //if (pos)
        //{
        //    *pos = '\0';
        //}

        iD = PI_ConnectUSB(PI_stage.c_str());
        connType = "USB";
        break;
    }

    if (iD < 0)
    {
        ReportError(iD);
        std::cout << "Connect " << connType << ": ERROR" << std::endl;
    }
}

bool PI_STAGE::WaitForMotionDone(int ID, std::string axis)
{
    BOOL isMoving = TRUE;
    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    while (isMoving)
    {
        std::unique_lock crit(command_protect);
        PI_IsMoving(ID, axis.c_str(), &isMoving);
        crit.unlock();
        if (!isMoving)
        {
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::cout << "Move time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[us]" << std::endl;
        }

        Sleep(1);
    }

    return true;
}

bool PI_STAGE::WaitForTrigger(int ID, const int* channels ,BOOL* values, int numChan, bool* running)
{
    BOOL waiting = true;
    
    while (waiting && *running)
    {
        std::unique_lock crit(command_protect);
        if (!PI_qDIO(ID, channels, values, numChan)) {
            std::cout << "Error reading pins" << std::endl;
            return false;
        }
        //else {
        //    std::cout << "Waiting For Trigger" << std::endl;
        //}
        crit.unlock();
        if (values[0])
        {
            waiting = false;
        }

        //Sleep(1);
    }

    return true;
}

void PI_STAGE::queryPos()
{

    BOOL result;
    std::unique_lock crit(command_protect);
    result = PI_qPOS(iD, sAxis.c_str(), &position);
    crit.unlock();
    if (!result) {
        std::cout << "Can't query position!!!" << std::endl;
    }
    else {
        data->_pos_label->set_text(std::to_string(position));
        data->_view->refresh();
    }
}

void PI_STAGE::PrintControllerIdentification(int iD)
{
    char szIDN[200];

    if (!PI_qIDN(iD, szIDN, 199))
    {
        std::runtime_error("qIDN failed. Exiting.");
    }
    std::cout << "qIDN returned: " << szIDN << std::endl;
}

void PI_STAGE::moveRel(double displace)
{
    BOOL result;
    double moveTo = position + displace;
    if (moveTo < minPos) {
        moveTo = 0.0;
    }
    else if (moveTo > maxPos) {
        moveTo = maxPos;
    }
    std::unique_lock crit(command_protect);
    result = PI_MOV(iD, sAxis.c_str(), &moveTo);
    crit.unlock();
    if (!result) {
        std::cout << "Failed to Move!!!" << std::endl;
    }
    else {
        WaitForMotionDone(iD, sAxis);
        data->_view->refresh();
        queryPos();
    }
}

void PI_STAGE::moveAbs(double newPos)
{
    BOOL result;
    if (newPos < minPos) {
        newPos = 0.0;
    }
    else if (newPos > maxPos) {
        newPos = maxPos;
    }
    std::unique_lock crit(command_protect);
    result = PI_MOV(iD, sAxis.c_str(), &newPos);
    crit.unlock();
    if (!result) {
        std::cout << "Failed to Move!!!" << std::endl;
    }
    else {
        WaitForMotionDone(iD, sAxis);
        data->_view->refresh();
        queryPos();
    }
}

void PI_STAGE::runSequence()
{
    if(trigger_thread_ptr != nullptr && trigger_thread_ptr->joinable() && !trigger_thd_run){
        trigger_thread_ptr->join(); 
        /*std::cout << "before delete" << std::endl;
        delete[]trigger_thread_ptr;
        std::cout << "aftter delete" << std::endl;
        trigger_thread_ptr = nullptr;*/
    }
    auto seq_thread = [this]()
    {
        this->data->returnPos = this->position;
        std::cout << "Pos: " << 0 << " ";
        this->moveRel(this->data->startDisplace);
        std::cout << "Stage Moved To Start Position" << std::endl;

        uint32_t wait_final_frame = std::ceil(1 / this->data->usb_dat->fpsVal * this->data->usb_dat->min_frames*1000);
        std::cout << "Wait Final Frame: " << (int)wait_final_frame << "ms" << std::endl;
        signal_slm->notify_one(); // Make SURE SLM is waiting for trigger
        this->data->usb_dat->outgoing.flags |= START_CAPTURE;


        int i = 0;
        while(this->trigger_thd_run){
            std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            std::cout << "Pos: " << i + 1 << " ";
            WaitForTrigger(this->iD,this->piChannelsArray,this->pbValueArray,this->iArraySize, &this->trigger_thd_run);
            this->moveRel(this->data->stepSize);

            ++i;
            if (i >= this->data->usb_dat->positions - 1) { // Steps are positions - 1 starting at 0;
                this->trigger_thd_run = false;
                std::cout << "The big sleep!!!!" << std::endl;
                Sleep(wait_final_frame); // Sleep so that the stage doesn't move while capturing the last position.

            }
            std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            std::cout << "Total time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[us]" << std::endl;
        }
        this->moveAbs(this->data->returnPos);
        //trigger_thd_run = false;
        std::cout << "Trig finished" << std::endl;
    };
    //if (trigger_thread_ptr = nullptr) {
        trigger_thd_run = true;
        trigger_thread_ptr = new std::thread(seq_thread);
        std::cout << "trigger thread started" << std::endl;
    //}
    //else {
    //    std::cout << "error running trigger thread" << std::endl;
    //}
}

void PI_STAGE::runLapseSequence()
{
    if (trigger_thread_ptr != nullptr && trigger_thread_ptr->joinable() && !trigger_thd_run) {
        trigger_thread_ptr->join();
        /*std::cout << "before delete" << std::endl;
        delete[]trigger_thread_ptr;
        std::cout << "aftter delete" << std::endl;
        trigger_thread_ptr = nullptr;*/
    }
    auto seq_thread = [this]()
    {
        this->data->returnPos = this->position;
        std::cout << "Pos: " << 0 << " ";
        this->moveRel(this->data->startDisplace);
        std::cout << "Stage Moved To Start Position" << std::endl;

        //Sleep(100);
        double startPosition = this->data->returnPos + this->data->startDisplace;
        uint32_t sleep_time = std::ceil((this->data->usb_dat->lapseVal - (1 / this->data->usb_dat->fpsVal * this->data->usb_dat->min_frames + 0.025) * (this->data->usb_dat->positions - 1))*1000);
        uint32_t wait_final_frame = std::ceil(1 / this->data->usb_dat->fpsVal * this->data->usb_dat->min_frames*1000);
        std::cout << "sleep_time: " << (int)sleep_time << "ms" << std::endl;
        std::cout << "Wait Final Frame: " << (int)wait_final_frame << "ms" << std::endl;
        int count = 0;
        std::cout << "starting count: " << count << std::endl;
        signal_slm->notify_one(); // Make SURE SLM is waiting for trigger
        this->data->usb_dat->outgoing.flags |= START_CAPTURE;

        while (count < this->data->usb_dat->lapse_counts) {
            int i = 0;
            std::cout << "lapse: " << count << std::endl;
            this->trigger_thd_run = true;

            while (this->trigger_thd_run) {
                //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                std::cout << "Pos: " << i + 1 << " ";
                WaitForTrigger(this->iD, this->piChannelsArray, this->pbValueArray, this->iArraySize, &this->trigger_thd_run);
                this->moveRel(this->data->stepSize);

                //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                //std::cout << "Total time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[us]" << std::endl;

                ++i;
                if (i == this->data->usb_dat->positions - 1) { // Steps are positions - 1 starting at 0;
                    this->trigger_thd_run = false;
                    Sleep(wait_final_frame); // Sleep so that the stage doesn't move while capturing the last position.
                    this->moveAbs(startPosition);
                    std::cout << "Stage Moved To Start Position, Waiting for next lapse." << std::endl;
                    Sleep(sleep_time - 10);
                    count++;
                    std::cout << "starting count: " << count << std::endl;
                    signal_slm->notify_one(); // Make SURE SLM is waiting for trigger
                    this->data->usb_dat->outgoing.flags |= START_CAPTURE;
                }
            }
        } 
        this->data->usb_dat->start_stop_butt_ptr->reset();
        this->moveAbs(this->data->returnPos);
        this->data->usb_dat->trigger_running;
        //trigger_thd_run = false;
        std::cout << "Trig finished" << std::endl;
    };
    //if (trigger_thread_ptr = nullptr) {
    trigger_thd_run = true;
    trigger_thread_ptr = new std::thread(seq_thread);
    std::cout << "lapse trigger thread started" << std::endl;
    //}
    //else {
    //    std::cout << "error running trigger thread" << std::endl;
    //}
}

void PI_STAGE::SetServoState(int ID, std::string axis, BOOL state)
{
    if (!PI_SVO(ID, axis.c_str(), &state))
    {
        throw std::runtime_error("SVO failed. Exiting");
    }

    std::cout << "Turned servo " << (state ? "on" : "off") << std::endl;
}

void PI_STAGE::ReportError(int iD)
{
    int err = PI_GetError(iD);
    char szErrMsg[300];

    if (PI_TranslateError(err, szErrMsg, 299))
    {
        std::cout << "Error " << err << " occurred: " << szErrMsg << std::endl;
    }
}