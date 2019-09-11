#include "Display.hpp"


//plot map1 line in 2D
void fill_map1(State_1& map1, std::vector<ccmpl::Point>& line){
  line.clear();
  for(auto& c : map1.tw.content){
    line.push_back({c.x , c.y});
  }
}

void fill_map2(State_2& map2, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  double i = 0;
  for(auto& p:map2.tw.content){
    ccmpl::RGB col(p,p*p,(1-p)*(1-p));
    colored_dots.push_back({{i,0.},col});
    i+=0.002;
  }
}

//plot bmus pos and color on same space

void fill_bmus(Results& results, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(int i = 0 ; i< results.bmu_w1.size();i++){
    double bmu = results.bmu_w2[i];
    ccmpl::RGB col(bmu,bmu*bmu,(1- bmu)*(1- bmu));
    colored_dots.push_back({{results.bmu_w1[i].x, results.bmu_w1[i].y},col});
  }
}

void fill_bmus_2(Results& results, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(int i = 0 ; i< results.bmu_w1_closed.size();i++){
    double bmu = results.bmu_w2_closed[i];
    ccmpl::RGB col(bmu,bmu*bmu,(1- bmu)*(1- bmu));
    colored_dots.push_back({{results.bmu_w1_closed[i].x, results.bmu_w1_closed[i].y},col});
  }
}



void fill_inputs(Results& results, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(int i = 0 ; i< results.inputsy.size();i++){
    double inp2 = results.inputsy[i];
    ccmpl::RGB col(inp2,inp2*inp2,(1-inp2)*(1-inp2));
    colored_dots.push_back({{results.inputsx[i].x, results.inputsx[i].y},col});
  }
}

void fill_colorbar(std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(double i = 0 ; i< 1.; i+=0.005){
    ccmpl::RGB col(i,i*i,(1-i)*(1-i));
    colored_dots.push_back({{0,i},col});
  }
}


void plot_positions(std::string& flags,ccmpl::chart::Layout& display,
  State_1& map1, State_2& map2, Results& results){

  display().title = "Map representation in input space";
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::line("'r'",         std::bind(fill_map1, std::ref(map1), _1));                                    flags += "#";

  display++;

  display().title = "Map 2 Weigths";
  display().xtitle  = "Position in map";
  display()   = ccmpl::view2d({0.,1.2}, {-0.2,0.2}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::confetti("s=100",         std::bind(fill_map2,std::ref(map2), _1));                                 flags += "#";
  display++;

  display().title = "colorbar";
  display()   = ccmpl::view2d({-0.2,0.2}, {0,1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::confetti("s=50",         std::bind(fill_colorbar, _1));                                            flags += "#";
  display++;

  display().title = "bmus weights positions, with both inputs";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "x";
  display().ytitle  = "y";
  display() += ccmpl::confetti("s=10", std::bind(fill_bmus, std::ref(results), _1)) ;                                      flags += "#";

  display++;
  display().title = "bmus weights positions without y input";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "x";
  display().ytitle  = "y";
  display() += ccmpl::confetti("s=50", std::bind(fill_bmus_2, std::ref(results), _1)) ;                                      flags += "#";

  display++;

  display().title = "Inputs";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()  += ccmpl::confetti("s=50",         std::bind(fill_inputs, std::ref(results), _1));                             flags += "#";

}
