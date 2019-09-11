#pragma once
#include <xsom.hpp>
#include <ccmpl.hpp>
#include "2D_state.hpp"


#define MEM_PLOT 100



void fill_bmus(Results& results, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_bmus_2(Results& results, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_inputs(Results& results, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_colorbar(std::vector<ccmpl::ColorAt>& colored_dots);

void plot_positions(std::string& flags,ccmpl::chart::Layout& display,State_1& map1, State_2& map2, Results& results);

void fill_input(State_1& map1, ccmpl::Point& dot);

void fill_bmu(State_1& map1, ccmpl::Point& dot);

void fill_bmu_2(State_2& map2, ccmpl::Point& dot);

void fill_maps( State_1& map1, State_2& map2, std::vector<ccmpl::ColorAt>& colored_dots );

ccmpl::RGB getcolor(double v,double vmin,double vmax);

void plot_activity(std::string& flags,ccmpl::chart::Layout& display,
  State_1& map1, State_2& map2, Results& results);

void fill_input_2(State_2& map2, std::vector<ccmpl::ColorAt>& colored_dots);
