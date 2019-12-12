#pragma once
#include <xsom.hpp>
#include <ccmpl.hpp>
#include "Simple_state.hpp"


#define MEM_PLOT 100

void fill_bar(const double& value, double& bar_pos);

void fill_anyline(std::vector<double>& data,std::vector<ccmpl::Point>& line);

void fill_anyline_2(std::vector<double>& datax, std::vector<double>& datay,std::vector<ccmpl::Point>& line);

void fill_err_lines(std::vector<double>& x_input, std::vector<double>& y_input, std::vector<double>& datax, std::vector<double>& datay, std::vector<std::vector<ccmpl::Point>>& lines);

void fill_dot(State& map1, State& map2, ccmpl::Point& dot);

void fill_input(State& map1, State& map2, ccmpl::Point& dot);

void fill_map(State& map1, State& map2,State& map3, std::vector<ccmpl::Point>& line);

void fill_map_2(State& map1, State& map2,State& map3, std::vector<ccmpl::Point>& line);

void plot_positions(std::string& flags,ccmpl::chart::Layout& display,State& map1, State& map2, State& map3, Results& results);
