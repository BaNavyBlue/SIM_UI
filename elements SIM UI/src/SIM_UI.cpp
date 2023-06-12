#include <SIM_UI.hpp>
#include <SLM.hpp>

using namespace cycfi::elements;

// Main window background color
auto constexpr bkd_color = rgba(35, 35, 37, 255);
auto background = box(bkd_color);

constexpr auto bred = colors::red.opacity(0.4);

constexpr auto bgreen = colors::green.level(0.7).opacity(0.4);
constexpr auto bblue = colors::blue.opacity(0.4);
constexpr auto brblue = colors::royal_blue.opacity(0.4);
constexpr auto pgold = colors::gold.opacity(0.8);

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



SIM_UI::~SIM_UI() {
   threads_alive = false;

   //signal_toggle.notify_one();
   //toggle_thread->join();
   if (_SLM_STATUS) {
      SLM.SLM.keep_alive = false;
      if(!SLM.SLM.is_running) signal_slm.notify_one();
      std::cout << "Joining Thread" << std::endl;
      SLM.join_slm_thread();
   }
}

void SIM_UI::stop_slm(){
   std::cout << "trying to join thread" << std::endl;
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

auto SIM_UI::make_basic_interface()
{
   auto fr = [](auto&& el, float top = 10)
   {
      return margin(
         { 10, top, 10, 10 },
         layer(
            margin({ 10, 5, 10, 5 }, std::move(el)),
            frame{}
         )
      );
   };

   auto eh = [=](char const* txt)
   {
      return fr(halign(0.5, heading{ txt }), 0);
   };

   auto el = [=](auto const& label_)
   {
      return fr(halign(0.5, label_));
   };

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
   std::string my_string = "SIM_UI Aligator";
   char const* font_family = "文泉驿微米黑, \"WenQuanYi Micro Hei\"";

   static float const grid[] = { 0.32, 1.0 };

   auto  laser_button1 = radio_button("Blue 488nm");
   auto  laser_button2 = radio_button("Green 561nm");
   auto  laser_button3 = radio_button("Both Alternated");
   _toggle_blanking = share(toggle_button("Blanking", 1.0, bred));

   _toggle_blanking->on_click =
      [&](bool) mutable
   {
      //signal_toggle.notify_one();
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

   _indicator_butt_ptr = share(button("Start/Stop", 1.0, pgold));
   //auto indicator_butt_ptr = button("Push Me!");
   auto tbutton = toggle_button("Arm Stage", 1.0, bgreen);
   //auto lbutton = latching_button("Latching Button", 1.0, bgreen);
   SLM_DATA._slm_button = share(latching_button("Start SLM", 1.0, bgreen));

   SLM_DATA._slm_button->on_click =
      [&](bool) mutable
   {
      start_slm();
   };

   _indicator_butt_ptr->on_click =
      [&](bool) mutable
   {
      if (_trigger_running) {
         _trigger_running = false;
         SLM.SLM.is_running = false;
      }
      else {
         _trigger_running = true;
         signal_slm.notify_one();
      }
   };

   auto PANEL_BUTTONS =
      pane("Woopty Woo",
         group("Captain Obvious",
            margin({ 10, 10, 20, 20 },
               top_margin(25,
                  vtile(
                     top_margin(10, align_left(hold(SLM_DATA._slm_button))),
                     top_margin(10, align_left(tbutton)),
                     top_margin(10, align_left(hold(_indicator_butt_ptr)))
                  )
               )
            )
         ))
      ;

   auto indicator_color = get_theme().indicator_color;

   auto Zstage_pos = [=](auto text)
   {
      _stage_pos = share(label(text));
      return align_center(hold(_stage_pos));
   };

   auto stage_interface = [=](auto text, auto input) {
      return left_margin(20, hgrid(/*grid,*/ right_margin(5, label(text).text_align(canvas::right)), hsize(50, input), label("µm").text_align(canvas::center)));
   };

   auto small_step = input_box(small_step_str);
   auto large_step = input_box(large_step_str);
   auto new_pos = input_box(jump_to_str);

   auto small_ax_step = input_box(small_step_str);
   auto large_ax_step = input_box(large_step_str);
   auto new_ax_pos = input_box(jump_to_str);




   auto STAGE_BUTTONS =
      pane("Stage Control",
         group("-        Focus Stage        +",
            margin({ 10, 10, 20, 10 },
               vtile(
                  top_margin(35,
                     htile(
                        align_right(icon_button(icons::angle_double_left, 1.2, bblue)),
                        align_left(icon_button(icons::angle_left, 1.2, bblue)),
                        align_center(std::move(Zstage_pos("0.0"))),
                        align_right(icon_button(icons::angle_right, 1.2, bblue)),
                        align_left(icon_button(icons::angle_double_right, 1.2, bblue))
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

   auto SLM_STAGE_BUTTONS =
      pane("Stage Control",
         group("-        SLM Stage        +",
            margin({ 10, 10, 20, 10 },
               vtile(
                  top_margin(35,
                     htile(
                        align_right(icon_button(icons::angle_double_left, 1.2, bblue)),
                        align_left(icon_button(icons::angle_left, 1.2, bblue)),
                        align_center(std::move(Zstage_pos("0.0"))),
                        align_right(icon_button(icons::angle_right, 1.2, bblue)),
                        align_left(icon_button(icons::angle_double_right, 1.2, bblue))
                     )
                  ), top_margin(20,
                     htile(
                        /*align_center(*/stage_interface("Small: ", small_ax_step.first),//),
                        /*align_center(*/stage_interface("Large: ", large_ax_step.first)//)
                     )
                  ), top_margin(20,
                     align_center(hsize(160, stage_interface("Jump to: ", new_ax_pos.first)))
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

   

   auto my_input = [=](auto caption, auto input)
   {
      return right_margin(10, hgrid(grid, my_label(caption), input));
   };

   // This is an example on how to add an on_text callback:
   auto fps = input_box("5.0");
   fps.second->on_text =
      [input = fps.second.get()](std::string_view text)
   {
      if (text == "$1000000")
      {
         input->set_text("Thank You!!!");
         input->select_all();
      }
   };

   auto zstep = input_box("0");
   zstep.second->on_text =
      [input = zstep.second.get()](std::string_view text)
   {
      if (text == "$1000000")
      {
         input->set_text("Thank You!!!");
         input->select_all();
      }
   };

   auto text_input =
      pane("Text Input",
         margin({ 10, 5, 10, 5 },
            vtile(
               my_input("FPS:", fps.first),
               my_input("Z-steps", zstep.first)
            )
         ))
      ;

   auto labels =
      top_margin(20, pane("Labels",
         vtile(
            el(label("Hello, Universe. This is Elements.")
               .font(font_descr{ "Open Sans" }.semi_bold())
               .font_color(colors::antique_white)
               .font_size(18)
            ),
            el(
               vtile(
                  label("A cross-platform,")
                  .text_align(canvas::center),
                  label("fine-grained,")
                  .text_align(canvas::left),
                  label("highly modular C++ GUI library.")
                  .text_align(canvas::right),
                  label("Based on a GUI framework written in the mid 90s named Pica."),
                  label("Now, Joel rewrote my code using modern C++17.")
               )
            )
         )))
      ;

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
            top_margin(20, pane("Message", std::move(my_message(my_string)))),
            empty()
         ), vtile(
            align_top(hmin_size(350, margin({ 20, 20, 20, 20 }, STAGE_BUTTONS))),
            align_top(hmin_size(350, margin({ 20, 20, 20, 20 }, SLM_STAGE_BUTTONS)))
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
         make_page(SLM_TAB)
      )
      , tab("Interface")
      , tab("Timing")
      , tab("SLM Conf")
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

   _win.on_close = [this]() { stop(); };

   _view.content(

      make_tabs(_view),

      background     // Replace background with your main element,
                     // or keep it and add another layer on top of it.
   );
   start_test_thread();
}
