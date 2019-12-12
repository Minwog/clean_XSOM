#include "Display_retro.hpp"



void fill_bar(const double& value, double& bar_pos) {bar_pos = value;}

void plot_positions(std::string& flags,ccmpl::chart::Layout& display,
  State& map1, State& map2, State& map3, Results& results){
  display().title = "Map 1 weights";
  display().xtitle  = "Position in map";
  display().ytitle  = "Weight";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map1.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map1.x), _1));                     flags += "#";
  display()  += ccmpl::line("'r'",         std::bind(&TWeights::fill_line, std::ref(map1.tw), _1));        flags += "#";
  display()  += ccmpl::line("'g'",         std::bind(&CWeights::fill_line, std::ref(map1.cw), _1));        flags += "#";

  display++;

  display().title = "Map 2 Weigths";
  display().xtitle  = "Position in map";
  display().ytitle  = "Weight";
  display() = ccmpl::legend("['cortical map', 'thalamic map'], loc='upper right'");
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map2.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map2.x), _1));                     flags += "#";
  display()  += ccmpl::line("'g'",         std::bind(&CWeights::fill_line, std::ref(map2.cw), _1));        flags += "#";
  display()  += ccmpl::line("'r'",         std::bind(&TWeights::fill_line, std::ref(map2.tw), _1));        flags += "#";
  display++;


  display().title = "Map 3 Weigths";
  display().xtitle  = "Position in map";
  display().ytitle  = "Weight";
  display() = ccmpl::legend("['cortical map', 'thalamic map'], loc='upper right'");
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map3.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map3.x), _1));                     flags += "#";
  display()  += ccmpl::line("'g'",         std::bind(&CWeights::fill_line, std::ref(map3.cw), _1));        flags += "#";
  display()  += ccmpl::line("'r'",         std::bind(&TWeights::fill_line, std::ref(map3.tw), _1));        flags += "#";
  display++;

}
