#pragma once
#include <xsom.hpp>
#include <ccmpl.hpp>
#include "State.hpp"

#define NB_W 100
#define NB_H 100
#define STEP 1


void fill_anyline(std::vector<Input>& datax,std::vector<ccmpl::Point>& line);
void fill_err_lines(std::vector<Input>& x_input, std::vector<Input>& datax, std::vector<std::vector<ccmpl::Point>>& lines);
void plot_positions(std::string& flags,ccmpl::chart::Layout& display,State& map1, Results& results);
void fill_map(State& map1, std::vector<ccmpl::Point>& line);
