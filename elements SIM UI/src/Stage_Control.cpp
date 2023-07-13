#include "Stage_Control.h"

//Stage::Stage(stage_type) {
//
//}

bool Stage::start_stage(stage_type stage_t) {
	if (stage_t == PI) {
		auto make_PI_thread = [&]() {
			thread_started = true;
			if (PIStage.PIStage_Thread(data)) {
				thread_started = false;
				std::cout << "PI thread failed" << std::endl;
				return false;
			}
		};
		stage_thread_ptr = new std::thread(make_PI_thread);
		return true;
	}
	else {
		return false;
	}
}

void Stage::join_stage_thread() {
	if (stage_thread_ptr->joinable()) {
		std::cout << "thread is joinable" << std::endl;
		stage_thread_ptr->join();
	}
	else {
		delete[]stage_thread_ptr;
	}
}

