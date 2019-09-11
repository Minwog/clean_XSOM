#pragma once
#include <xsom.hpp>
#include <ccmpl.hpp>
#include "Simple_state.hpp"


#define MEM_PLOT 100


void fill_map1(State_1& map1, std::vector<ccmpl::Point>& line);

void fill_map2(State_2& map2, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_bmus(Results& results, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_bmus_2(Results& results, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_inputs(Results& results, std::vector<ccmpl::ColorAt>& colored_dots);

void fill_colorbar(std::vector<ccmpl::ColorAt>& colored_dots);

void plot_positions(std::string& flags,ccmpl::chart::Layout& display,State_1& map1, State_2& map2, Results& results);
