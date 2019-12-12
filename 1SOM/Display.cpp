#include "Display.hpp"


void fill_anyline(std::vector<Input>& datax,std::vector<ccmpl::Point>& line){
  line.clear();
  for(auto& p :datax){
    line.push_back({p.x,p.y});
  }
}


void fill_err_lines(std::vector<Input>& x_input, std::vector<Input>& datax, std::vector<std::vector<ccmpl::Point>>& lines){
  lines.clear();
  std::vector<ccmpl::Point> tmp;
  for(int i = 0;i<datax.size();i++){
    tmp.clear();
    ccmpl::Point inp((x_input[i]).x,x_input[i].y);
    ccmpl::Point bmu(datax[i].x,datax[i].y);
    tmp.push_back(inp);
    tmp.push_back(bmu);

    lines.push_back(tmp);
  }
}

void fill_map(State& map1, std::vector<ccmpl::Point>& line){
  line.clear();
  for(auto& p:map1.tw.content){
    line.push_back({p.x,p.y});
  }
}


void plot_positions(std::string& flags,ccmpl::chart::Layout& display,
  State& map1, Results& results){
  display().title = "Map 1 weights";
  display().xtitle  = "weight x";
  display().ytitle  = "weight y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::equal, ccmpl::span::placeholder);
  display()  += ccmpl::lines("'r'",         std::bind(xsom::tab2d::fill_lines<NB_W, NB_H, STEP>, std::ref(map1.tw), _1));        flags += "#";

  display++;

  display().title = "bmus weights x, y positions, with both inputs";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::equal, ccmpl::span::placeholder);
  display().xtitle  = "Map 1 dimension";
  display().ytitle  = "Map 2 dimension";
  display()  += ccmpl::lines("'b--'",              std::bind(fill_err_lines,std::ref(results.inputsx),std::ref(results.bmu_w1), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'k'",         std::bind(fill_anyline,std::ref(results.inputsx), _1)); flags += "#";
  display()  += ccmpl::dots("color = 'b'",         std::bind(fill_anyline,std::ref(results.bmu_w1), _1)); flags += "#";

}
