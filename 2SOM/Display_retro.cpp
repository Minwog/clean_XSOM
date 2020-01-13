#include "Display_retro.hpp"



void fill_bar(const double& value, double& bar_pos) {bar_pos = value;}

void fill_anyline(std::vector<double>& data,std::vector<ccmpl::Point>& line){
  line.clear();
  int i=0;
  for(auto d : data){
    line.push_back({(double) i, d});
    i++;
  }
}

void fill_anyline_2(std::vector<double>& datax, std::vector<double>& datay,std::vector<ccmpl::Point>& line){
  line.clear();
  for(int i = 0;i<datax.size();i++){
    try
    {
        line.push_back({datax[i],datay[i]});
   }
   catch ( const std::out_of_range & )
  {
         std::cerr << "Erreur : débordement de mémoire.\n";
    }
  }
}


void fill_err_lines(std::vector<double>& x_input, std::vector<double>& y_input, std::vector<double>& datax, std::vector<double>& datay, std::vector<std::vector<ccmpl::Point>>& lines){
  lines.clear();
  std::vector<ccmpl::Point> tmp;
  for(int i = 0;i<datax.size();i++){
    tmp.clear();
    ccmpl::Point inp(x_input[i],y_input[i]);
    ccmpl::Point bmu(datax[i],datay[i]);
    tmp.push_back(inp);
    tmp.push_back(bmu);
    lines.push_back(tmp);
  }
}


void fill_dot(State& map1, State& map2, ccmpl::Point& dot) {
  dot = {map1.tw(map1.bmu),map2.tw(map2.bmu)};
}



void fill_input(State& map1, State& map2, ccmpl::Point& dot) {
  dot = {map1.x,map2.x};
}

void fill_map(State& map1, State& map2,std::vector<ccmpl::Point>& line){
  line.clear();
  for(int i = 0; i< MAP_SIZE ; i++){
    double pos = i/500.;
    double val1 = map1.tw(map2.cw(pos));
    double val2 = map2.tw(pos);
    line.push_back({val1,val2});
  }
}

void fill_map_dots(State& map1, State& map2,std::vector<ccmpl::Point>& line){
  line.clear();
  for(int i = 0; i< MAP_SIZE ; i+=10){
    double pos = i/500.;
    double val1 = map1.tw(map2.cw(pos));
    double val2 = map2.tw(pos);
    line.push_back({val1,val2});
  }
}

void fill_map_2(State& map1, State& map2,std::vector<ccmpl::Point>& line){
  line.clear();
  for(int i = 0; i< MAP_SIZE ; i++){
    double pos = i/500.;
    double val1 = map2.tw(map1.cw(pos));
    double val2 = map1.tw(pos);
    line.push_back({val2,val1});
  }
}

void fill_map_2_dots(State& map1, State& map2,std::vector<ccmpl::Point>& line){
  line.clear();
  for(int i = 0; i< MAP_SIZE ; i+=10){
    double pos = i/500.;
    double val1 = map2.tw(map1.cw(pos));
    double val2 = map1.tw(pos);
    line.push_back({val2,val1});
  }
}



void plot_positions(std::string& flags,ccmpl::chart::Layout& display,
  State& map1, State& map2, Results& results){
  display().title = "Map 1 weights";
  display().xtitle  = "Position in map";
  display().ytitle  = "Weight";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map1.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map1.x), _1));                     flags += "#";
  display()  += ccmpl::line("'r'",         std::bind(&TWeights::fill_line, std::ref(map1.tw), _1));        flags += "#";
  display()  += ccmpl::line("'g'",         std::bind(&CWeights::fill_line, std::ref(map1.cw), _1));        flags += "#";
  display()  += ccmpl::line("'r--'",         std::bind(&Acts::fill_line, std::ref(map1.ta), _1));        flags += "#";
  display()  += ccmpl::line("'g--'",         std::bind(&Acts::fill_line, std::ref(map1.ca), _1));        flags += "#";
  display()  += ccmpl::line("'b--'",         std::bind(&Acts::fill_line, std::ref(map1.ma), _1));        flags += "#";

  display++;

  display().title = "Map 2 Weigths";
  display().xtitle  = "Position in map";
  display().ytitle  = "Weight";
  display() = ccmpl::legend("['contextual map', 'external map'], loc='upper right'");
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::line("'g'",         std::bind(&CWeights::fill_line, std::ref(map2.cw), _1));        flags += "#";
  display()  += ccmpl::line("'r'",         std::bind(&TWeights::fill_line, std::ref(map2.tw), _1));        flags += "#";
  display()  += ccmpl::vbar("'k'",         std::bind(fill_bar, std::ref(map2.bmu), _1));                   flags += "#";
  display()  += ccmpl::hbar("'k'",         std::bind(fill_bar, std::ref(map2.x), _1));                     flags += "#";
    display()  += ccmpl::line("'r--'",         std::bind(&Acts::fill_line, std::ref(map2.ta), _1));        flags += "#";
  display()  += ccmpl::line("'g--'",         std::bind(&Acts::fill_line, std::ref(map2.ca), _1));        flags += "#";
  display()  += ccmpl::line("'b--'",         std::bind(&Acts::fill_line, std::ref(map2.ma), _1));        flags += "#";

  display++;

  display().title = "bmus weights x, y positions, with both inputs";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "Map 1 dimension";
  display().ytitle  = "Map 2 dimension";
  display()  += ccmpl::lines("'b--'",              std::bind(fill_err_lines,std::ref(results.inputsx), std::ref(results.inputsy),std::ref(results.bmu_w1), std::ref(results.bmu_w2), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'k'",         std::bind(fill_anyline_2,std::ref(results.inputsx), std::ref(results.inputsy), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'b'",         std::bind(fill_anyline_2,std::ref(results.bmu_w1), std::ref(results.bmu_w2), _1)); flags += "#";
  //display()  += ccmpl::dots("color = 'b'",         std::bind(fill_bmus,std::ref(map1), std::ref(map2), _1)); flags += "#";
  //display()  += ccmpl::dots("color = 'k'",         std::bind(fill_inputs,std::ref(map1), std::ref(map2), _1)); flags += "#";

  display++;
  display().title = "bmus weights x, y positions without y input";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "Map 1 dimension";
  display().ytitle  = "Map 2 dimension";
  //display()  += ccmpl::line("color = 'k'",         std::bind(fill_circle, 0.5, 0.5,0.5, 100, _1)); flags += "#";
  display()  += ccmpl::lines("'r--'",             std::bind(fill_err_lines,std::ref(results.inputsx_closed), std::ref(results.inputsy_closed),std::ref(results.bmu_w1_closed), std::ref(results.bmu_w2_closed), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'k'",        std::bind(fill_anyline_2,std::ref(results.inputsx_closed), std::ref(results.inputsy_closed), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'r'",        std::bind(fill_anyline_2,std::ref(results.bmu_w1_closed), std::ref(results.bmu_w2_closed), _1)); flags += "#";

    display++;

  display().title = "bmu weight prediction";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "$\\xi^x$";
  display().ytitle  = "$\\omega(p\\star^x)$";
  //display()  += ccmpl::line("color = 'k'",         std::bind(fill_circle, 0.5, 0.5,0.5, 100, _1)); flags += "#";
  display()  += ccmpl::dots("color = 'k'",        std::bind(fill_anyline_2,std::ref(results.inputsx), std::ref(results.bmu_w1), _1)); flags += "#";
  display ++;
  display().title = "bmu weight prediction";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "$\\xi^y$";
  display().ytitle  = "$\\omega(p\\star^y)$";
  //display()  += ccmpl::line("color = 'k'",         std::bind(fill_circle, 0.5, 0.5,0.5, 100, _1)); flags += "#";
  display()  += ccmpl::dots("color = 'k'",        std::bind(fill_anyline_2,std::ref(results.inputsy), std::ref(results.bmu_w2), _1)); flags += "#";


/*  display().title = "bmus weights x, y positions without x input";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "Map 1 dimension";
  display().ytitle  = "Map 2 dimension";
  //display()  += ccmpl::line("color = 'k'",         std::bind(fill_circle, 0.5, 0.5,0.5, 100, _1)); flags += "#";
  display()  += ccmpl::lines("'r--'",             std::bind(fill_err_lines,std::ref(results.inputsx_closed2), std::ref(results.inputsy_closed2),std::ref(results.bmu_w1_closed2), std::ref(results.bmu_w2_closed2), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'k'",        std::bind(fill_anyline_2,std::ref(results.inputsx_closed2), std::ref(results.inputsy_closed2), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'r'",        std::bind(fill_anyline_2,std::ref(results.bmu_w1_closed2), std::ref(results.bmu_w2_closed2), _1)); flags += "#";

  display++;*/

  /*display().title = "Visualisation in 2D";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::equal, ccmpl::span::placeholder);
  display().xtitle  = "Map 1 dimension";
  display().ytitle  = "Map 2 dimension";
  display()  += ccmpl::line("'k-'",         std::bind(fill_map, std::ref(map1), std::ref(map2), _1)); flags += "#";
  display()  += ccmpl::dots("color='r', s = 20",         std::bind(fill_map_dots, std::ref(map1), std::ref(map2), _1)); flags += "#";

  display++;
  display().title = "Second visualisation in 2D";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::equal, ccmpl::span::placeholder);
  display().xtitle  = "Map 1 dimension";
  display().ytitle  = "Map 2 dimension";
  display()  += ccmpl::line("'k-'",         std::bind(fill_map_2, std::ref(map1), std::ref(map2), _1)); flags += "#";
  display()  += ccmpl::dots("color='r', s = 20",         std::bind(fill_map_2_dots, std::ref(map1), std::ref(map2), _1)); flags += "#";*/

/*  display ++;
  display().title = "Error evolution through epochs";
  display()   = ccmpl::view2d(ccmpl::limit::fit, ccmpl::limit::fit, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "Batch number";
  display().ytitle  = "mean squared error";
  display()  += ccmpl::line("'k'", std::bind(fill_anyline, std::ref(results.mean_error_batch), _1)); flags += "#";*/

}
