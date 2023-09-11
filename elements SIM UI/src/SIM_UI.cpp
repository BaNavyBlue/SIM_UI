#include <SIM_UI.hpp>
#include <SLM.hpp>
#include "Stage_Control.h"

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

constexpr auto bred = colors::red.opacity(0.4);

constexpr auto bgreen = colors::green.level(0.7).opacity(0.4);
constexpr auto bblue = colors::blue.opacity(0.4);
constexpr auto brblue = colors::royal_blue.opacity(0.4);
constexpr auto pgold = colors::gold.opacity(0.8);

bool isDecimal(const std::string test) {
    bool decimal = true;
    if (test.size() < 1) return false;
    for (auto it = test.begin(); it < test.end(); ++it) {
        if ((std::isdigit(*it) || *it == '.' || *it == '-')) {
            decimal = true;
        }
        else {
            return false;
        }
    }
    return decimal;
}

bool isInteger(const std::string test) {
    bool integer = true;
    if (test.size() < 1) return false;
    for (auto it = test.begin(); it < test.end(); ++it) {
        if (std::isdigit(*it)) {
            integer = true;
        }
        else {
            return false;
        }
    }
    return integer;
}

auto box = draw(
   [](context const& ctx)
   {
      auto& c = ctx.canvas;

      c.begin_path();
      c.round_rect(ctx.bounds, 4);
      c.fill_style(colors::gold.opacity(0.8));
      c.fill();
   }
);

std::string const text =
"We are in the midst of an intergalatic condensing of beauty that will "
"clear a path toward the planet itself. The quantum leap of rebirth is "
"now happening worldwide. It is time to take healing to the next level. "
"Soon there will be a deepening of chi the likes of which the infinite "
"has never seen. The universe is approaching a tipping point. This "
"vision quest never ends. Imagine a condensing of what could be. "
"We can no longer afford to live with stagnation. Suffering is born "
"in the gap where stardust has been excluded. You must take a stand "
"against discontinuity.\n\n"

"Without complexity, one cannot dream. Stagnation is the antithesis of "
"life-force. Only a seeker of the galaxy may engender this wellspring of hope."
"Yes, it is possible to eliminate the things that can destroy us, but not "
"without wellbeing on our side. Where there is delusion, faith cannot thrive. "
"You may be ruled by desire without realizing it. Do not let it eliminate "
"the growth of your journey.\n\n"

"--New-Age Bullshit Generator"
;



SIM_UI::~SIM_UI() 
{
   threads_alive = false;

   //signal_toggle.notify_one();
   //toggle_thread->join();
   if (USB_DATA.usb_running) {
       USB.stopUSB();
   }
   if (_SLM_STATUS) {
      SLM.SLM.keep_alive = false;
      /*if (!SLM.SLM.is_running)*/ signal_slm.notify_one();
      std::cout << "Joining Thread" << std::endl;
      SLM.join_slm_thread();
   }
   if (_PI_STATUS) {
       PIStage.PIStage.threadIsAlive = false;
       std::cout << "Joining PI Thread" << std::endl;
       PIStage.join_stage_thread();
   }
   if (_THOR_STATUS) {
       THORStage.THORStage.threadIsAlive = false;
       std::cout << "Joining THOR Thread" << std::endl;
       THORStage.join_stage_thread();
   }
}


void SIM_UI::stop_slm()
{
   std::cout << "trying to join thread" << std::endl;
   SLM.SLM.keep_alive = false;
   /*if (!SLM.SLM.is_running)*/ signal_slm.notify_one();
   SLM.join_slm_thread();
}

void SIM_UI::start_slm()
{
   
   SLM.data = &SLM_DATA;
   if (SLM.thread_started) stop_slm();
   SLM.start_meadowlark();
   _SLM_STATUS = true;
   SLM_DATA._msg_label->set_text("!!!SLM Starting!!!");
   refresh_view();
      //_start_latch_ptr->
}

void SIM_UI::start_PI_stage()
{
    PIStage.data = &PI_DATA;
    _PI_STATUS = PIStage.start_stage(PI);
    if (!_PI_STATUS) {
        std::cout << "Problem Starting PI stage" << std::endl;
    } 
}

void SIM_UI::start_THOR_stage()
{
    THORStage.data = &THOR_DATA;
    _THOR_STATUS = THORStage.start_stage(THOR);
    if (!_THOR_STATUS) {
        std::cout << "Problem Starting Thor stage" << std::endl;
    }
}

void SIM_UI::start_USB()
{
    USB.thd_dat = &USB_DATA;
    USB.startUSB();
}

void SIM_UI::start_count_lapse()
{
    if (count_lapse_thd != nullptr && count_lapse_thd->joinable() && !this->USB_DATA.trigger_running) {
        count_lapse_thd->join();
        /*std::cout << "before delete" << std::endl;
        delete[]trigger_thread_ptr;
        std::cout << "aftter delete" << std::endl;
        trigger_thread_ptr = nullptr;*/
    }
    auto count_lapse_thread = [this]() {
        uint32_t sleep_time = std::ceil((this->USB_DATA.lapseVal) * 1000);
        std::cout << "sleep_time: " << (int)sleep_time << "ms" << std::endl;

        int i = 0;
        //std::cout << "lapse: " << count << std::endl;
        //this->trigger_thd_run = true;
        bool lapse_running = true;
        while (lapse_running) {
            //std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
            std::cout << "Lapse count: " << i << std::endl;

            //std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
            //std::cout << "Total time = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[us]" << std::endl;
            Sleep(sleep_time);
            this->PIStage.PIStage.signal_slm->notify_one(); // Make SURE SLM is waiting for trigger
            this->USB_DATA.outgoing.flags |= START_CAPTURE;
            ++i;
            if (i == USB_DATA.lapse_counts - 1) { // Steps are positions - 1 starting at 0;
                lapse_running = false;



                std::cout << "Timelapse Concluded: " << std::endl;


            }
        }
        Sleep(1000); //Lets make sure last image isn't missed somehow.
        this->USB_DATA.start_stop_butt_ptr->reset();
    };
    count_lapse_thd = new std::thread(count_lapse_thread);
    std::cout << "Count Timelapse Started" << std::endl;
}



void SIM_UI::start_test_thread()
{
  /* auto see_toggle_happen = [&]() {
      int i = 1;
      while (threads_alive) {
         std::unique_lock slp(sleep_test);
         signal_toggle.wait(slp);
         if (threads_alive) {
            _msg_label->set_text("Been Toggled " + std::to_string(i) + " Times!");
            refresh_view();
            std::cout << "Been Toggled! " << (int)i << std::endl;
            i++;
         }
      }
   };

   toggle_thread = std::unique_ptr<std::thread>(new std::thread(see_toggle_happen));*/

}

void SIM_UI::refresh_view() {
   _view.refresh();
}

auto SIM_UI::make_stage_tab()
{
    auto PI_check_box = check_box("Triggered Sequence");
    auto start_z = input_box(std::to_string(PI_DATA.startDisplace));
    auto end_z = input_box(std::to_string(PI_DATA.endDisplace));
    auto step_size = input_box(std::to_string(PI_DATA.stepSize));
    auto positions = input_box(std::to_string(USB_DATA.positions));
    auto center_range = (check_box("Center Range"));

    auto PI_mode_button1 = (radio_button("Solve Step Size By Range/(Positions - 1)"));
    auto PI_mode_button2 = (radio_button("Solve Range By (Positions - 1) * (Step Size)"));
    PI_mode_button1.select(true);

    PI_check_box.value(false);
    center_range.value(true);

    PI_check_box.on_click =
        [this](bool click_state)
    {
        this->USB_DATA.triggerStage = click_state;
        if (click_state) {
            this->USB_DATA.count_run_state = false;
            this->USB_DATA.outgoing.steps = this->USB_DATA.positions - 1;
            //std::unique_lock lck(this->USB_DATA.usb_crit);
            
            this->USB_DATA.outgoing.mode = 0;
            this->USB_DATA.outgoing.mode = Z_MODE;
            this->USB_DATA.outgoing.flags |= (CHANGE_Z_STEPS | SET_RUN_MODE);
            //lck.unlock();
            float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
            if (this->USB_DATA.triggerStage && !this->free_run_state) {
                if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                    this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                    this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                }
            }
        }
        else {
            //std::unique_lock lck(this->USB_DATA.usb_crit);
            
            this->USB_DATA.outgoing.mode = 0;
            this->USB_DATA.outgoing.mode = COUNT_MODE;
            this->USB_DATA.outgoing.flags |= (SET_RUN_MODE);
            this->USB_DATA.count_run_state = true;
            //this->USB.sendData();
            //lck.unlock();
        }
        this->USB.change_lapse_count();
    };

    center_range.on_click =
        [this](bool click_state)
    {
        this->PI_center_range = click_state;
    };

    PI_mode_button1.on_click =
        [this](bool click_state)
    {
        //if (click_state) {
            std::cout << "One" << std::endl;
        //}
        this->PI_trigger_mode1 = click_state;
    };

    PI_mode_button2.on_click =
        [this](bool click_state)
    {
        //if (click_state) {
            std::cout << "Two" << std::endl;
        //}
        this->PI_trigger_mode2 = click_state;
    };

    start_z.second->on_enter =
        [this, input = start_z.second.get(), smol = &PI_DATA, output = step_size.second.get()](std::string_view text)
    {
        //std::size_t found = text.find();
        std::string testMe(text);
        if (!isDecimal(testMe))
        {
            std::cout << "Is not decimal" << std::endl;
            input->set_text(std::to_string(smol->startDisplace));
            /*input->set_text("Thank You!!!");
            input->select_all();*/
        }
        else {
            
            if (this->PI_trigger_mode1 && (this->USB_DATA.positions > 1)) {
                smol->startDisplace = std::stod(testMe);
                smol->stepSize = (smol->endDisplace - smol->startDisplace) / (this->USB_DATA.positions - 1);
                output->set_text(std::to_string(smol->stepSize));
            }
            else {
                input->set_text(std::to_string(smol->startDisplace));
            }
        }
    };

    end_z.second->on_enter =
        [this, input = end_z.second.get(), smol = &PI_DATA, output = step_size.second.get()](std::string_view text)
    {
        //std::size_t found = text.find();
        std::string testMe(text);
        if (!isDecimal(testMe))
        {
            std::cout << "Is not decimal" << std::endl;
            input->set_text(std::to_string(smol->endDisplace));
            /*input->set_text("Thank You!!!");
            input->select_all();*/
        }
        else {
            
            if (this->PI_trigger_mode1 && (this->USB_DATA.positions > 1)) {
                smol->endDisplace = std::stod(testMe);
                smol->stepSize = (smol->endDisplace - smol->startDisplace) / (this->USB_DATA.positions - 1);
                output->set_text(std::to_string(smol->stepSize));
            }
            else {
                input->set_text(std::to_string(smol->endDisplace));
            }
        }
    };

    step_size.second->on_enter =
        [this, input = step_size.second.get(), smol = &PI_DATA, start = start_z.second.get(), end = end_z.second.get()](std::string_view text)
    {
        //std::size_t found = text.find();
        std::string testMe(text);
        if (!isDecimal(testMe))
        {
            std::cout << "Is not decimal" << std::endl;
            input->set_text(std::to_string(smol->stepSize));
            /*input->set_text("Thank You!!!");
            input->select_all();*/
        }
        else {
            
            if (this->PI_trigger_mode2 && (this->USB_DATA.positions > 1)) {
                smol->stepSize = std::stod(testMe);
                if (this->PI_center_range) {
                    std::cout << "Center Range True" << std::endl;
                    double range = (this->USB_DATA.positions - 1) * smol->stepSize;
                    smol->startDisplace = -range / 2;
                    smol->endDisplace = range / 2;
                    start->set_text(std::to_string(smol->startDisplace));
                    end->set_text(std::to_string(smol->endDisplace));
                    //end->select_all();
                }
                else {
                    smol->startDisplace = 0;
                    smol->endDisplace = (this->USB_DATA.positions - 1) * smol->stepSize;
                    start->set_text(std::to_string(smol->startDisplace));
                    end->set_text(std::to_string(smol->endDisplace));
                    //end->select_all();
                }
            }
            else {
                std::cout << "Mode Button 2 not selected" << std::endl;
                input->set_text(std::to_string(smol->stepSize));
                //input->select_all();
            }
        }
    };
    
    positions.second->on_enter =
        [this, input = positions.second.get(), smol = &PI_DATA, start = start_z.second.get(), end = end_z.second.get(), stSize = step_size.second.get()](std::string_view text)
    {
        //std::size_t found = text.find();
        std::string testMe(text);
        if (!isInteger(testMe))
        {
            std::cout << "Is not integer" << std::endl;
            input->set_text(std::to_string(this->USB_DATA.positions));
            /*input->set_text("Thank You!!!");
            input->select_all();*/
        }
        else {
            if (std::stod(testMe) > 0) {
                this->USB_DATA.positions = std::stod(testMe);
            }
            else {
                input->set_text(std::to_string(this->USB_DATA.positions));
            }
            if (this->PI_trigger_mode2) {

                if (this->PI_center_range) {
                    std::cout << "Center Range True" << std::endl;
                    double range = (this->USB_DATA.positions - 1) * smol->stepSize;
                    smol->startDisplace = -range / 2;
                    smol->endDisplace = range / 2;
                    start->set_text(std::to_string(smol->startDisplace));
                    start->select_all();
                    end->set_text(std::to_string(smol->endDisplace));
                    //end->select_all();
                }
                else {
                    smol->startDisplace = 0;
                    smol->endDisplace = (this->USB_DATA.positions - 1) * smol->stepSize;
                    start->set_text(std::to_string(smol->startDisplace));
                    end->set_text(std::to_string(smol->endDisplace));
                    //end->select_all();
                }
            }
            else {
                if (this->USB_DATA.positions > 1) {
                    smol->stepSize = (smol->endDisplace - smol->startDisplace) / (this->USB_DATA.positions - 1);
                    stSize->set_text(std::to_string(smol->stepSize));
                }
                else {
                    stSize->set_text("0");
                }
            }
            this->USB_DATA.outgoing.steps = this->USB_DATA.positions - 1;
            //std::unique_lock lck(this->USB_DATA.usb_crit);
            this->USB_DATA.outgoing.flags |= CHANGE_Z_STEPS;

            float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
            if (this->USB_DATA.triggerStage && !this->free_run_state) {
                if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                    this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                    this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                }
            }
            else if (this->USB_DATA.lapseVal < lapse_ideal) {
                this->USB_DATA.lapseVal = lapse_ideal;
                this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
            }
            this->USB.change_lapse_count();
            //lck.unlock();
        }
    };

    auto trig_interface = [=](auto text, auto input) {
        return left_margin(20, hgrid(/*grid,*/ right_margin(5, label(text).text_align(canvas::right)), hsize(50, input), label("µm").text_align(canvas::center)));
    };

    

    auto  PI_interface =
        pane("PI Stage",
            group("Trigger Options",
                margin({ 10, 10, 20, 20 },
                    top_margin(25,
                        vtile(
                            top_margin(10, align_left(PI_check_box)),
                            top_margin(10, align_left(PI_mode_button1)),
                            top_margin(10, align_left(PI_mode_button2)),
                            top_margin(10, align_left(left_margin(20,center_range))),
                            top_margin(10, (trig_interface("Start Z:±   ", start_z.first))),
                            top_margin(10, (trig_interface("End Z:±     ", end_z.first))),
                            top_margin(20, (trig_interface("Step Size:± ", step_size.first))),
                            top_margin(10, (left_margin(20, hgrid(right_margin(5, label("Positions:   ").text_align(canvas::right)), hsize(50, positions.first), label("").text_align(canvas::center)))))
                        )
                    )
                )
            ))
        ;
    return 
        margin(
            { 10, 0, 10, 10 },
            htile(
                vtile(
                    htile(
                        hmin_size(250, margin({ 20, 20, 20, 20 }, PI_interface))
                    )
            )
        ));
}

/// <summary>
/// This Section Defines the primary UI page and the element callbacks.
/// It is sort of large and unweildy.
/// </summary>
/// <returns></returns>

auto SIM_UI::make_basic_interface()
{
   //auto fr = [](auto&& el, float top = 10)
   //{
   //   return margin(
   //      { 10, top, 10, 10 },
   //      layer(
   //         margin({ 10, 5, 10, 5 }, std::move(el)),
   //         frame{}
   //      )
   //   );
   //};

   //auto eh = [=](char const* txt)
   //{
   //   return fr(halign(0.5, heading{ txt }), 0);
   //};

   //auto el = [=](auto const& label_)
   //{
   //   return fr(halign(0.5, label_));
   //};

   /*auto icons =
      margin({ 10, 0, 10, 10 },
         htile(
            align_center(icon{ icons::docs }),
            align_center(icon{ icons::right }),
            align_center(icon{ icons::trash }),
            align_center(icon{ icons::block }),
            align_center(icon{ icons::cw }),
            align_center(icon{ icons::attention }),
            align_center(icon{ icons::menu }),
            align_center(icon{ icons::lightbulb }),
            align_center(icon{ icons::sliders }),
            align_center(icon{ icons::exchange })
         )
      );*/
   std::string my_string = "SIM_UI Alligator";

   char const* font_family = "文泉驿微米黑, \"WenQuanYi Micro Hei\"";

   static float const grid[] = { 0.32, 1.0 };

   auto  laser_button1 = radio_button("Blue 488nm");
   auto  laser_button2 = radio_button("Green 561nm");
   auto  laser_button3 = radio_button("Both Alternated");

   laser_button1.on_click =
       [this](bool click_state)
   {
       if (click_state) {
           std::cout << "BLUE" << std::endl;

           this->USB_DATA.outgoing.mode = 0;
           this->USB_DATA.outgoing.mode |= BLUE_LASER;
           this->USB_DATA.outgoing.flags |= SET_LASER_MODE;
       }
   };

   laser_button2.on_click =
       [this](bool click_state)
   {
       if (click_state) {
           std::cout << "GREEN" << std::endl;

           this->USB_DATA.outgoing.mode = 0;
           this->USB_DATA.outgoing.mode |= GREEN_LASER;
           this->USB_DATA.outgoing.flags |= SET_LASER_MODE;
       }
   };

   laser_button3.on_click =
       [this](bool click_state)
   {
       if (click_state) {
           std::cout << "BOTH" << std::endl;

           this->USB_DATA.outgoing.mode = 0;
           this->USB_DATA.outgoing.mode |= BOTH_LASERS;
           this->USB_DATA.outgoing.flags |= SET_LASER_MODE;
       }
   };

  
   _toggle_blanking = share(toggle_button("Blanking", 1.0, bred));

   _toggle_blanking->on_click =
      [&](bool toggle_state) mutable
   {
       if (!blanked) {
           blanked = true;
           USB_DATA.outgoing.mode = 0;
           USB_DATA.outgoing.mode |= START_BLANKING;
           USB_DATA.outgoing.flags |= TOGGLE_BLANKING;
           std::cout << "Blanking On" << std::endl;
       }
       else {
           blanked = false;
           USB_DATA.outgoing.mode = 0;
           USB_DATA.outgoing.mode |= STOP_BLANKING;
           USB_DATA.outgoing.flags |= TOGGLE_BLANKING;
       }
   };

   laser_button1.select(true);

   auto  laser_buttons =
      pane("Illumination",
         group("Lasers",
            margin({ 10, 10, 20, 20 },
               top_margin(25,
                  vtile(
                     top_margin(10, align_left(laser_button1)),
                     top_margin(10, align_left(laser_button2)),
                     top_margin(10, align_left(laser_button3)),
                     top_margin(20, hold(_toggle_blanking))
                  )
               )
            )
         ))
      ;

   auto  SIM_button1 = radio_button("3 Phase");
   auto  SIM_button2 = radio_button("5 Phase");
   auto  SIM_button3 = radio_button("7 Phase");
   auto  SIM_button4 = radio_button("None");

   SIM_button2.select(true);

   auto OverideSIMRadio = [=,butt1 = SIM_button1, butt2 = SIM_button2, butt3 = SIM_button3, butt4 = SIM_button4](auto state)
   {
       
   };

   SIM_button1.on_click =
       [&](bool click_state)
   {
       if (click_state) {
           //if (!this->SLM.SLM.is_running) {
               this->USB_DATA.SIM_angles = 3;
               this->USB_DATA.SIM_phases = 3;
               this->USB_DATA.min_frames = 9;
               if (this->USB_DATA.triggerStage) {
                   this->USB_DATA.min_frames *= this->USB_DATA.positions;
               }
               float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
               if (this->USB_DATA.triggerStage && !this->free_run_state) {
                   if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                       this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                       this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                   }

               }
               else if (this->USB_DATA.lapseVal < lapse_ideal) {
                   this->USB_DATA.lapseVal = lapse_ideal;
                   this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
               }
               this->USB.change_lapse_count();
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= TWO_BEAM;
               this->USB_DATA.outgoing.flags |= SET_SIM_MODE;
           //}
           //else {
           //    switch (this->sim_state) {
           //        case THREE_PHASE:
           //            SIM_button1.select(true);
           //            break;
           //        case FIVE_PHASE_THREE_BEAM:
           //            SIM_button2.select(true);
           //            break;
           //        case SEVEN_PHASE_THREE_BEAM:
           //            SIM_button3.select(true);
           //            break;
           //        case NO_SIM:
           //            SIM_button4.select(true);
           //            break;
           //    }
           //}
       }
   };

   SIM_button2.on_click =
       [&](bool click_state)
   {
       
       if (click_state) {
           //if (!this->SLM.SLM.is_running) {
               this->SLM.SLM.stPath = this->SLM.SLM.stripePath3BEAM;
               this->SLM.SLM.reloadImageBuffer();
               this->USB_DATA.SIM_angles = 3;
               this->USB_DATA.SIM_phases = 5;
               this->USB_DATA.min_frames = 15;
               if (this->USB_DATA.triggerStage) {
                   this->USB_DATA.min_frames *= this->USB_DATA.positions;
               }
               float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
               if (this->USB_DATA.triggerStage && !this->free_run_state) {
                   if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                       this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                       this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                   }

               }
               else if (this->USB_DATA.lapseVal < lapse_ideal) {
                   this->USB_DATA.lapseVal = lapse_ideal;
                   this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
               }
               this->USB.change_lapse_count();
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= THREE_BEAM;
               this->USB_DATA.outgoing.flags |= SET_SIM_MODE;
           //}
           //else {
           //    switch (this->sim_state) {
           //        case THREE_PHASE:
           //            SIM_button1.select(true);
           //            break;
           //        case FIVE_PHASE_THREE_BEAM:
           //            SIM_button2.select(true);
           //            break;
           //        case SEVEN_PHASE_THREE_BEAM:
           //            SIM_button3.select(true);
           //            break;
           //        case NO_SIM:
           //            SIM_button4.select(true);
           //            break;
           //    }
           //}
       }
   };

   SIM_button3.on_click =
       [&](bool click_state)
   {
       if (click_state) {
           //if (!this->SLM.SLM.is_running) {
               this->SLM.SLM.stPath = this->SLM.SLM.stripePath7Phase;
               this->SLM.SLM.reloadImageBuffer();
               this->USB_DATA.SIM_angles = 3;
               this->USB_DATA.SIM_phases = 7;
               this->USB_DATA.min_frames = 21;
               if (this->USB_DATA.triggerStage) {
                   this->USB_DATA.min_frames *= this->USB_DATA.positions;
               }
               float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
               if (this->USB_DATA.triggerStage && !this->free_run_state) {
                   if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                       this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                       this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                   }

               }
               else if (this->USB_DATA.lapseVal < lapse_ideal) {
                   this->USB_DATA.lapseVal = lapse_ideal;
                   this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
               }
               this->USB_DATA.outgoing.mode = 0;
               if (this->free_run_state) {
                   this->USB_DATA.outgoing.mode |= SEVEN_FREE;
               }
               else {
                   this->USB_DATA.outgoing.mode |= SEVEN_PHASE;
               }

               this->USB.change_lapse_count();
               this->USB_DATA.outgoing.flags |= SET_SIM_MODE;
           //}
           /*else {
               switch (this->sim_state) {
                   case THREE_PHASE:
                       SIM_button1.select(true);
                       break;
                   case FIVE_PHASE_THREE_BEAM:
                       SIM_button2.select(true);
                       break;
                   case SEVEN_PHASE_THREE_BEAM:
                       SIM_button3.select(true);
                       break;
                   case NO_SIM:
                       SIM_button4.select(true);
                       break;
               }
           }*/
       }
       this->seven_phase_mode = click_state;
   };

   SIM_button4.on_click =
       [&](bool click_state)
   {
       if (click_state) {
           //if (!this->SLM.SLM.is_running) {
               this->USB_DATA.SIM_angles = 1;
               this->USB_DATA.SIM_phases = 1;
               this->USB_DATA.min_frames = 1;
               if (this->USB_DATA.triggerStage) {
                   this->USB_DATA.min_frames *= this->USB_DATA.positions;
               }
               float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
               if (this->USB_DATA.triggerStage && !this->free_run_state) {
                   if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                       this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                       this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                   }

               }
               else if (this->USB_DATA.lapseVal < lapse_ideal) {
                   this->USB_DATA.lapseVal = lapse_ideal;
                   this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
               }
               this->USB.change_lapse_count();
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= NO_SIM_Z_ONLY;
               this->USB_DATA.outgoing.flags |= SET_SIM_MODE;
           //}
           //else {
           //    switch (this->sim_state) {
           //        case THREE_PHASE:
           //            SIM_button1.select(true);
           //            break;
           //        case FIVE_PHASE_THREE_BEAM:
           //            SIM_button2.select(true);
           //            break;
           //        case SEVEN_PHASE_THREE_BEAM:
           //            SIM_button3.select(true);
           //            break;
           //        case NO_SIM:
           //            SIM_button4.select(true);
           //            break;
           //
           //    }
           //}
       }
   };

   auto  SIM_buttons =
      pane("SIM Phases",
         group("SIM Mode",
            margin({ 10, 10, 20, 20 },
               top_margin(25,
                  vtile(
                     top_margin(10, align_left(SIM_button1)),
                     top_margin(10, align_left(SIM_button2)),
                     top_margin(10, align_left(SIM_button3)),
                     top_margin(10, align_left(SIM_button4))
                  )
               )
            )
         ))
      ;

   USB_DATA.start_stop_butt_ptr = share(button("Start/Stop", 1.0, pgold));
   //auto indicator_butt_ptr = button("Push Me!");
   //auto tbutton = toggle_button("Arm Stage", 1.0, bgreen);
   //auto lbutton = latching_button("Latching Button", 1.0, bgreen);
   SLM_DATA._slm_button = share(latching_button("Start SLM", 1.0, bgreen));

   auto free_run = check_box("Free Running");
   free_run.value(false);

   free_run.on_click =
       [this](bool click_state)
   {
       if (click_state) {
           this->USB_DATA.outgoing.mode = 0;
           this->USB_DATA.outgoing.mode |= FREE_RUN;
           this->USB_DATA.outgoing.flags |= SET_RUN_MODE;
           if (this->seven_phase_mode) {
               Sleep(100);
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= SEVEN_FREE;
               this->USB_DATA.outgoing.flags |= SET_SIM_MODE;
           }
           this->USB_DATA.count_run_state = false;
           this->free_run_state = true;
       }
       else {
           if (this->seven_phase_mode) {
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= SEVEN_PHASE;
               this->USB_DATA.outgoing.flags |= SET_SIM_MODE;
               this->free_run_state = false;
           } else
           if (this->USB_DATA.triggerStage) {
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= Z_MODE;
               this->USB_DATA.outgoing.flags |= SET_RUN_MODE;
               this->free_run_state = false;
           }
           else {
               this->USB_DATA.outgoing.mode = 0;
               this->USB_DATA.outgoing.mode |= COUNT_MODE;
               this->USB_DATA.outgoing.count = this->USB_DATA.count;
               this->USB_DATA.outgoing.flags |= SET_RUN_MODE;
               this->USB_DATA.count_run_state = true;
               this->free_run_state = false;
               this->USB.change_lapse_count();
           }

       }

   };

   SLM_DATA._slm_button->on_click =
      [&](bool) mutable
   {
      start_slm();
   };

   USB_DATA.start_stop_butt_ptr->on_click =
      [&](bool) mutable
   {
       std::cout << "start/stop button pressed" << std::endl;
      if (USB_DATA.trigger_running) {
         //std::unique_lock lck(this->USB_DATA.usb_crit);
         USB_DATA.outgoing.flags |= STOP_CAPTURE;
         //lck.unlock();
         USB_DATA.trigger_running = false;
         SLM.SLM.is_running = false;
         if (_PI_STATUS && USB_DATA.triggerStage) {
             PIStage.PIStage.trigger_thd_run = false;
         }
      }
      else {

         USB_DATA.trigger_running = true;
         if (_THOR_STATUS && seven_phase_mode && !free_run_state) {
             THORStage.THORStage.runSevenPhaseSeq();
             signal_slm.notify_one();
             USB_DATA.outgoing.flags |= START_CAPTURE;
             std::cout << "starting 7 phase capture" << std::endl;
         }
         else if (_PI_STATUS && USB_DATA.triggerStage && !free_run_state) {
             if (USB_DATA.run_time_lapse) {
                 PIStage.PIStage.runLapseSequence();
                 std::cout << "starting z-stack lapse Sequence" << std::endl;
             }
             else {
                 PIStage.PIStage.runSequence();
                 std::cout << "starting z-stack sequence" << std::endl;
             }
             //Sleep(50); //Give time for stage to move to start position
         }
         else if (USB_DATA.count_run_state && USB_DATA.run_time_lapse && !free_run_state) {
             std::cout << "calling count time lapse" << std::endl;
             start_count_lapse();
             std::unique_lock lck(this->USB_DATA.usb_crit);
             USB_DATA.outgoing.flags |= START_CAPTURE;
         }
         else {
             signal_slm.notify_one();
             USB_DATA.outgoing.flags |= START_CAPTURE;
         }


         //signal_slm.notify_one();
         //std::unique_lock lck(this->USB_DATA.usb_crit);
         //USB_DATA.outgoing.flags |= START_CAPTURE;
         //lck.unlock();
      }

   };

   auto PANEL_BUTTONS =
      pane("Woopty Woo",
         group("Captain Obvious",
            margin({ 10, 10, 20, 20 },
               top_margin(25,
                  vtile(
                     top_margin(10, align_left(hold(SLM_DATA._slm_button))),
                     
                     top_margin(10, align_left(hold(USB_DATA.start_stop_butt_ptr))),
                      top_margin(10, align_left(free_run))
                  )
               )
            )
         ))
      ;

   auto indicator_color = get_theme().indicator_color;

   auto PIstage_pos = [=](auto text)
   {
      PI_DATA._pos_label = share(label(text));
      return align_center(hold(PI_DATA._pos_label));
   };

   auto THORstage_pos = [=](auto text)
   {
       THOR_DATA._pos_label = share(label(text));
       return align_center(hold(THOR_DATA._pos_label));
   };

   auto stage_interface = [=](auto text, auto input) {
      return left_margin(20, hgrid(/*grid,*/ right_margin(5, label(text).text_align(canvas::right)), hsize(50, input), label("µm").text_align(canvas::center)));
   };

   auto stage_interface_mm = [=](auto text, auto input) {
       return left_margin(20, hgrid(/*grid,*/ right_margin(5, label(text).text_align(canvas::right)), hsize(50, input), label("mm").text_align(canvas::center)));
   };

   auto small_step = input_box(small_step_str);
   auto large_step = input_box(large_step_str);
   auto new_pos = input_box(jump_to_str);

   auto small_ax_step = input_box(small_step_str);
   auto large_ax_step = input_box(large_step_str);
   auto new_ax_pos = input_box(jump_to_str);

   small_step.second->on_enter =
       [input = small_step.second.get(), smol = &PIStage](std::string_view text)
   {   
       //std::size_t found = text.find();
       std::string testMe(text);
       if (!isDecimal(testMe))
       {
           std::cout << "Is not decimal" << std::endl;
           /*input->set_text("Thank You!!!");
           input->select_all();*/
       }
       else {
           smol->PIStage.smallStep = std::stod(testMe);
       }
   };

   large_step.second->on_enter =
       [input = large_step.second.get(), large = &PIStage](std::string_view text)
   {
       //std::size_t found = text.find();
       std::string testMe(text);
       if (!isDecimal(testMe))
       {
           std::cout << "Is not decimal" << std::endl;
           /*input->set_text("Thank You!!!");
           input->select_all();*/
       }
       else {
           large->PIStage.largeStep = std::stod(testMe);
       }
   };

   new_pos.second->on_enter =
       [input = new_pos.second.get(), dest = &PIStage](std::string_view text)
   {
       //std::size_t found = text.find();
       std::string testMe(text);
       if (!isDecimal(testMe))
       {
           std::cout << "Is not decimal" << std::endl;
           /*input->set_text("Thank You!!!");
           input->select_all();*/
       }
       else {
           dest->PIStage.moveAbs(std::stod(testMe));
       }
   };

   PI_DATA._leftS_button = share(icon_button(icons::angle_left, 1.2, bblue));
   PI_DATA._leftL_button = share(icon_button(icons::angle_double_left, 1.2, bblue));
   PI_DATA._RightS_button = share(icon_button(icons::angle_right, 1.2, bblue));
   PI_DATA._RightL_button = share(icon_button(icons::angle_double_right, 1.2, bblue));

   PI_DATA._leftS_button->on_click =
       [&](bool) mutable
   {
       PIStage.PIStage.moveRel(-PIStage.PIStage.smallStep);
   };

   PI_DATA._leftL_button->on_click =
       [&](bool) mutable
   {
       PIStage.PIStage.moveRel(-PIStage.PIStage.largeStep);
   };

   PI_DATA._RightS_button->on_click =
       [&](bool) mutable
   {
       PIStage.PIStage.moveRel(PIStage.PIStage.smallStep);
   };

   PI_DATA._RightL_button->on_click =
       [&](bool) mutable
   {
       PIStage.PIStage.moveRel(PIStage.PIStage.largeStep);
   };

   auto STAGE_BUTTONS =
      pane("Stage Control",
         group("-        Focus Stage        +",
            margin({ 10, 10, 20, 10 },
               vtile(
                  top_margin(35,
                     htile(
                        align_right(hold(PI_DATA._leftL_button)),
                        align_left(hold(PI_DATA._leftS_button)),
                        align_center(std::move(PIstage_pos("0.0"))),
                        align_right(hold(PI_DATA._RightS_button)),
                        align_left(hold(PI_DATA._RightL_button))
                     )
                  ), top_margin(20,
                     htile(
                        /*align_center(*/stage_interface("Small: ", small_step.first),//),
                        /*align_center(*/stage_interface("Large: ", large_step.first)//)
                     )
                  ), top_margin(20,
                     align_center(hsize(160, stage_interface("Jump to: ", new_pos.first)))
                  )
               )
            )
         )
      );

   small_ax_step.second->on_enter =
       [input = small_ax_step.second.get(), smol = &THORStage](std::string_view text)
   {
       //std::size_t found = text.find();
       std::string testMe(text);
       if (!isDecimal(testMe))
       {
           std::cout << "Is not decimal" << std::endl;
           /*input->set_text("Thank You!!!");
           input->select_all();*/
       }
       else {
           smol->THORStage.smallStep = std::stod(testMe);
       }
   };

   large_ax_step.second->on_enter =
       [input = large_ax_step.second.get(), large = &THORStage](std::string_view text)
   {
       //std::size_t found = text.find();
       std::string testMe(text);
       if (!isDecimal(testMe))
       {
           std::cout << "Is not decimal" << std::endl;
           /*input->set_text("Thank You!!!");
           input->select_all();*/
       }
       else {
           large->THORStage.largeStep = std::stod(testMe);
       }
   };

   new_ax_pos.second->on_enter =
       [input = new_ax_pos.second.get(), dest = &THORStage](std::string_view text)
   {
       //std::size_t found = text.find();
       std::string testMe(text);
       if (!isDecimal(testMe))
       {
           std::cout << "Is not decimal" << std::endl;
           /*input->set_text("Thank You!!!");
           input->select_all();*/
       }
       else {
           dest->THORStage.moveAbs(std::stod(testMe));
       }
   };


   THOR_DATA._leftS_button = share(icon_button(icons::angle_left, 1.2, bblue));
   THOR_DATA._leftL_button = share(icon_button(icons::angle_double_left, 1.2, bblue));
   THOR_DATA._RightS_button = share(icon_button(icons::angle_right, 1.2, bblue));
   THOR_DATA._RightL_button = share(icon_button(icons::angle_double_right, 1.2, bblue));

   THOR_DATA._leftS_button->on_click =
       [&](bool) mutable
   {
       THORStage.THORStage.moveRel(-THORStage.THORStage.smallStep);
   };

   THOR_DATA._leftL_button->on_click =
       [&](bool) mutable
   {
       THORStage.THORStage.moveRel(-THORStage.THORStage.largeStep);
   };

   THOR_DATA._RightS_button->on_click =
       [&](bool) mutable
   {
       THORStage.THORStage.moveRel(THORStage.THORStage.smallStep);
   };

   THOR_DATA._RightL_button->on_click =
       [&](bool) mutable
   {
       THORStage.THORStage.moveRel(THORStage.THORStage.largeStep);
   };


   auto THOR_STAGE_BUTTONS =
      pane("Stage Control",
         group("-        SLM Stage        +",
            margin({ 10, 10, 20, 10 },
               vtile(
                  top_margin(35,
                     htile(
                        align_right(hold(THOR_DATA._leftL_button)),
                        align_left(hold(THOR_DATA._leftS_button)),
                        align_center(std::move(THORstage_pos("0.0"))),
                        align_right(hold(THOR_DATA._RightS_button)),
                        align_left(hold(THOR_DATA._RightL_button))
                     )
                  ), top_margin(20,
                     htile(
                        /*align_center(*/stage_interface_mm("Small: ", small_ax_step.first),//),
                        /*align_center(*/stage_interface_mm("Large: ", large_ax_step.first)//)
                     )
                  ), top_margin(20,
                     align_center(hsize(160, stage_interface_mm("Jump to: ", new_ax_pos.first)))
                  )
               )
            )
         )
      );

   auto my_label = [=](auto text)
   {
      return right_margin(10, label(text).text_align(canvas::center));
   };

   auto my_message = [=](auto text)
   {
      SLM_DATA._msg_label = share(label(text));
      return align_center(hold(SLM_DATA._msg_label));
   };

   auto config_message = [=](auto text)
   {
       USB_DATA._config_label = share(label(text));
       return align_center(hold(USB_DATA._config_label));
   };

   

   auto my_input = [=](auto caption, auto input)
   {
       return left_margin(50, hgrid(grid, align_right(my_label(caption)), hsize(50,input)));// , label("sec").text_align(canvas::center)));
   };

   auto exp_input = [=](auto caption, auto input, auto input2)
   {
       return right_margin(10, hgrid(grid, my_label(caption), hsize(200,input), input2));
   };

   // This is an example on how to add an on_text callback:
   USB_DATA.fps = input_box("5.0");
   //USB_DATA.fps = input_box("5.0");
   USB_DATA.fps.second->on_enter =
      [this, input = USB_DATA.fps.second.get()](std::string_view text)
   {
      std::string test_txt(text);
      if (isDecimal(test_txt)&&std::stod(test_txt)>0.0)
      {
         this->USB_DATA.fpsVal = this->USB_DATA.outgoing.fps = std::stod(test_txt);
         this->USB_DATA.outgoing.flags |= CHANGE_FPS;
         input->set_text(text);
         float lapse_ideal = 1 / std::stod(test_txt) * this->USB_DATA.min_frames;
         if (this->USB_DATA.triggerStage && !this->free_run_state) {
             if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025*(this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                 this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                 this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
             }
         }
         else if (this->USB_DATA.lapseVal < lapse_ideal) {
             this->USB_DATA.lapseVal = lapse_ideal;
             this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
         }
         //input->select_all();
      }
      else {
          input->set_text(std::to_string(this->USB_DATA.fpsVal));
      }
   };
   USB_DATA.exp = input_box("0");
   //USB_DATA.exp = input_box("0");
   USB_DATA.exp.second->on_enter =
      [this, input = USB_DATA.exp.second.get()](std::string_view text)
   {
       std::string test_txt(text);
       if (isDecimal(test_txt))
      {
         this->USB_DATA.expVal = this->USB_DATA.outgoing.exposure = std::stod(test_txt);
         
         input->set_text(text);
         if (this->USB_DATA.arbitrary_exp) {
             this->USB_DATA.outgoing.mode = 0;
             this->USB_DATA.outgoing.mode |= ARB_EXP;
             float lapse_ideal = std::stod(test_txt) * this->USB_DATA.min_frames;
             if (this->USB_DATA.triggerStage && !this->free_run_state) {
                 if (this->USB_DATA.lapseVal < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                     this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                     this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
                 }
             }
             else if (this->USB_DATA.lapseVal < lapse_ideal) {
                 this->USB_DATA.lapseVal = lapse_ideal;
                 this->USB_DATA.lapse_period.second.get()->set_text(std::to_string(this->USB_DATA.lapseVal));
             }
         }
         else {
             this->USB_DATA.outgoing.mode &= ~ARB_EXP;
         }
         //input->select_all();
         this->USB.change_lapse_count();
         this->USB_DATA.outgoing.flags |= SET_EXPOSURE;
       }
       else {
           input->set_text(std::to_string(this->USB_DATA.expVal));
       }
   };

   USB_DATA.lapseVal = 1 / USB_DATA.fpsVal * USB_DATA.min_frames;
   USB_DATA.lapse_period = input_box(std::to_string(USB_DATA.lapseVal));
   auto lapse_duration = input_box("5.0");

   lapse_duration.second->on_enter = [this, input = lapse_duration.second.get()](std::string_view text)
   {
       std::string test_txt(text);
       if (isDecimal(test_txt) && std::stod(test_txt) > 0.0)
       {
           this->USB_DATA.lapse_time = std::stod(test_txt);
           if (this->USB_DATA.run_time_lapse) {
               this->USB.change_lapse_count();
           }
       }
   };

   // Arbitrary Exposure

   USB_DATA.lapse_period.second->on_enter =
       [this, input = USB_DATA.lapse_period.second.get()](std::string_view text)
   {
       std::string test_txt(text);
       if (isDecimal(test_txt) && std::stod(test_txt) > 0.0)
       {
           float lapse_ideal = 1 / this->USB_DATA.fpsVal * this->USB_DATA.min_frames;
           if (this->USB_DATA.triggerStage && !this->free_run_state) {
               if (std::stod(test_txt) < (lapse_ideal + 0.025 * (this->USB_DATA.positions - 1))) { // 0.025 is the stage wait time
                   this->USB_DATA.lapseVal = lapse_ideal + 0.025 * (this->USB_DATA.positions - 1);
                   input->set_text(std::to_string(this->USB_DATA.lapseVal));
               }
               else {
                   this->USB_DATA.lapseVal = std::stod(test_txt);
                   input->set_text(std::to_string(this->USB_DATA.lapseVal));
               }
           }
           else if (std::stod(test_txt) < lapse_ideal) {
               this->USB_DATA.lapseVal = lapse_ideal;
               input->set_text(std::to_string(this->USB_DATA.lapseVal));
           } else {
               this->USB_DATA.lapseVal = std::stod(test_txt);
               input->set_text(std::to_string(this->USB_DATA.lapseVal));
           }
           if (this->USB_DATA.run_time_lapse) {
               this->USB.change_lapse_count();
           }
       }
       
   };

   auto exp_mode_ptr = check_box("Arbitrary Exposure");

   exp_mode_ptr.value(false);
   exp_mode_ptr.on_click = 
       [this](bool click_state)
   {
       this->USB_DATA.arbitrary_exp = click_state;
   };

   auto frames = input_box(std::to_string(USB_DATA.count));

   frames.second->on_enter = [this, input = frames.second.get()](std::string_view text)
   {
       std::string test_txt(text);
       if (isInteger(test_txt) && std::stoi(test_txt) > 0) {
           this->USB_DATA.count = std::stoi(test_txt);
       }
       else {
           input->set_text(std::to_string(this->USB_DATA.count));
       }
       this->USB.change_lapse_count();
   };

   auto time_lapse_check = check_box("Time Lapse");

   time_lapse_check.on_click = [this](bool click_state) {
       if (click_state) {
           this->USB_DATA.run_time_lapse = true;
       }
       else {
           this->USB_DATA.run_time_lapse = false;
       }
       this->USB.change_lapse_count();
   };

   auto text_input =
      pane("Exposure Control",
         margin({ 10, 5, 10, 5 },
            vtile(
               htile(my_input("FPS (1/sec):", USB_DATA.fps.first),
                     my_input("lapse period(sec):", USB_DATA.lapse_period.first),
                     my_input("lapse duration(min):", lapse_duration.first),
                     time_lapse_check),
               top_margin(10, htile(my_input("Exposure (sec):", USB_DATA.exp.first),
                     align_left(exp_mode_ptr),
                     my_input("(SIM) Frames:", frames.first)))
            )
         ))
      ;

   //auto labels =
   //   top_margin(20, pane("Labels",
   //      vtile(
   //         el(label("Hello, Universe. This is Elements.")
   //            .font(font_descr{ "Open Sans" }.semi_bold())
   //            .font_color(colors::antique_white)
   //            .font_size(18)
   //         ),
   //         el(
   //            vtile(
   //               label("A cross-platform,")
   //               .text_align(canvas::center),
   //               label("fine-grained,")
   //               .text_align(canvas::left),
   //               label("highly modular C++ GUI library.")
   //               .text_align(canvas::right),
   //               label("Based on a GUI framework written in the mid 90s named Pica."),
   //               label("Now, Joel rewrote my code using modern C++17.")
   //            )
   //         )
   //      )))
   //   ;
   return
      margin(
         { 10, 0, 10, 10},
         htile(
         vtile(
            htile(
               hmin_size(250, margin({ 20, 20, 20, 20 }, laser_buttons)),
               hmin_size(175, margin({ 20, 20, 20, 20 }, SIM_buttons)),
               hmin_size(250, margin({ 20, 20, 20, 20 }, PANEL_BUTTONS))
               
            ),
            text_input,
            top_margin(20, pane("Messages", vtile(top_margin(1,std::move(my_message(my_string))), std::move(config_message("Taco Bell"))))),
            empty()
         ), vtile(
            align_top(hmin_size(350, margin({ 20, 20, 20, 20 }, STAGE_BUTTONS))),
            align_top(hmin_size(350, margin({ 20, 20, 20, 20 }, THOR_STAGE_BUTTONS)))
         )
         )
      );
}

auto SIM_UI::make_basic_text2()
{
   auto textbox = share(vport(static_text_box{ text }));
   return hmin_size(350, margin(
      { 10, 0, 10, 10 },
      hold(textbox)
   ));
}

auto SIM_UI::make_elements(TABS tab_value)
{
   if (tab_value == INTERFACE_TAB) {
      return
         layer(max_size({ 1280, 640 },
            margin({ 20, 10, 20, 10 },
               htile(
                  margin({ 20, 20, 20, 20 }, make_basic_interface()//),
                     //margin({ 20, 20, 20, 20 },
                        //pane("Text Box", make_basic_text2())
                  )
               )
            )
         ), frame{});
   }
   else if (tab_value == TIMING_TAB) {
      return
         layer(max_size({ 1280, 640 },
            margin({ 20, 10, 20, 10 },
               htile(
                  margin({ 20, 20, 20, 20 }, make_basic_text2()//),
                     //margin({ 20, 20, 20, 20 },
                        //pane("Text Box", make_basic_text2())
                  )
               )
            )
         ), frame{});
   }
   else if (tab_value == STAGE_CONTROL) {
       return
           layer(max_size({ 1280, 640 },
               margin({ 20, 10, 20, 10 },
                   htile(
                       margin({ 20, 20, 20, 20 }, make_stage_tab()//),
                           //margin({ 20, 20, 20, 20 },
                              //pane("Text Box", make_basic_text2())
                       )
                   )
               )
           ), frame{});
   }
   else {
      return layer(align_center_middle(label("SLM Conf").font_size(100)), frame{});
   }
}

auto SIM_UI::make_tabs(view& view_)
{
   auto make_page = [this](auto tabs)
   {
      // return layer(align_center_middle(label(text).font_size(100)), frame{});
      return make_elements(tabs);
   };

   return vnotebook(
       view_
       , deck(
           make_page(INTERFACE_TAB),
           make_page(TIMING_TAB),
           make_page(SLM_TAB),
           make_page(STAGE_CONTROL)
      )
      , tab("Interface")
      , tab("Timing")
      , tab("SLM Conf")
      , tab("Stage Conf")
   );
}

SIM_UI::SIM_UI(int argc, char* argv[])
   : app{ argc, argv,"SIM Scope Control", "com.cycfi.notebook" }
   , _win{ name() }
   , _view{ _win }
{
   SLM_DATA.signal_slm = &signal_slm;
   SLM_DATA._slm_mutex = &sleep_slm;
   SLM_DATA._view = &_view;
   PI_DATA._view = &_view;
   PI_DATA.signal_stage = &USB_DATA.signal_PI;
   PI_DATA.usb_dat = &USB_DATA;
   PIStage.PIStage.signal_slm = &signal_slm;
   THOR_DATA.usb_dat = &USB_DATA;
   THOR_DATA.signal_stage = &USB_DATA.signal_THOR;
   THOR_DATA._view = &_view;

   _win.on_close = [this]() { stop(); };

   _view.content(

      make_tabs(_view),

      background     // Replace background with your main element,
                     // or keep it and add another layer on top of it.
   );
   start_USB();
   start_PI_stage();
   start_THOR_stage();
   USB_DATA._config_label->set_text(USB.conf_string());
   //start_test_thread();
}
