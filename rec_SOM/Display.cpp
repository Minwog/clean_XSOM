#include "Display.hpp"



void fill_bar(const double& value, double& bar_pos) {bar_pos = value;}


void plot_positions(std::string& flags,ccmpl::chart::Layout& display,
  State& map1, Results& results){
  display().title = "Map1 weights";
  display().xtitle  = "External input";
  display().ytitle  = "Weight";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map1.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map1.x), _1));                     flags += "#";
  display()  += ccmpl::line("'r'",         std::bind(&TWeights::fill_line, std::ref(map1.tw), _1));        flags += "#";
  display()  += ccmpl::line("'r--'",         std::bind(&Acts::fill_line, std::ref(map1.ta), _1));          flags += "#";
  display()  += ccmpl::line("'b'",         std::bind(&Acts::fill_line, std::ref(map1.ma), _1));            flags += "#";


  display++;

  display().title = "Map 2 Weigths";
  display().xtitle  = "Contextual input";
  display().ytitle  = "Weight";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::line("'g'",         std::bind(&CWeights::fill_line, std::ref(map1.cw), _1));        flags += "#";
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map1.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map1._x), _1));                    flags += "#";
  display()  += ccmpl::line("'g--'",         std::bind(&Acts::fill_line, std::ref(map1.ca), _1));          flags += "#";
  display()  += ccmpl::line("'b'",         std::bind(&Acts::fill_line, std::ref(map1.ma), _1));            flags += "#";

}
