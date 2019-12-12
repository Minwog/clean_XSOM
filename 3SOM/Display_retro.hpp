#pragma once
#include <xsom.hpp>
#include <ccmpl.hpp>
#include "Simple_state.hpp"


#define MEM_PLOT 100

void fill_bar(const double& value, double& bar_pos);


void plot_positions(std::string& flags,ccmpl::chart::Layout& display,State& map1, State& map2,State& map3, Results& results);
