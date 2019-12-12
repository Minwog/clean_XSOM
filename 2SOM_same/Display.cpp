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
  State& map1,State& map2, Results& results){
  display().title = "Map 1 Weights";
  display().xtitle  = "weight x";
  display().ytitle  = "weight y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::equal, ccmpl::span::placeholder);
  display()  += ccmpl::line("'r'",                 std::bind(fill_map, std::ref(map1), _1 ));                       flags += "#";
  display()  += ccmpl::dots("color = 'b'",         std::bind(fill_anyline,std::ref(results.bmu_w1), _1));   flags += "#";

  display++;

  display().title = "Map2 weights";
  display()   = ccmpl::view2d({0.,1.1}, {0.,1.1}, ccmpl::aspect::equal, ccmpl::span::placeholder);
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()  += ccmpl::line("'r'",                  std::bind(fill_map, std::ref(map2), _1            ));        flags += "#";
  display()  += ccmpl::dots("color = 'b'",         std::bind(fill_anyline,std::ref(results.bmu_w2), _1)); flags += "#";

}
