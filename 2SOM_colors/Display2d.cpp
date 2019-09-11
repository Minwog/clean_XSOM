#include "Display2d.hpp"

#define NB_W 20
#define NB_H 20
#define STEP 1


//plot bmus pos and color on same space

void fill_bmus(Results& results, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(int i = 0 ; i< results.bmu_w1.size();i++){
    double bmu = results.bmu_w2[i];
    //ccmpl::RGB col(bmu,bmu*bmu,(1- bmu)*(1- bmu));
    ccmpl::RGB col = getcolor(bmu, 0,1);
    colored_dots.push_back({{results.bmu_w1[i].x, results.bmu_w1[i].y},col});
  }
}

void fill_bmus_2(Results& results, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(int i = 0 ; i< results.bmu_w1_closed.size();i++){
    double bmu = results.bmu_w2_closed[i];
    //ccmpl::RGB col(bmu,bmu*bmu,(1- bmu)*(1- bmu));
    ccmpl::RGB col = getcolor(bmu, 0,1);
    colored_dots.push_back({{results.bmu_w1_closed[i].x, results.bmu_w1_closed[i].y},col});
  }
}



void fill_inputs(Results& results, std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(int i = 0 ; i< results.inputsy.size();i++){
    double inp2 = results.inputsy[i];
  //  ccmpl::RGB col(inp2,inp2*inp2,(1-inp2)*(1-inp2));
  ccmpl::RGB col = getcolor(inp2, 0,1);
    colored_dots.push_back({{results.inputsx[i].x, results.inputsx[i].y},col});
  }
}

void fill_input(State_1& map1, ccmpl::Point& dot){
  dot = {map1.x.x, map1.x.y};
}

void fill_input_2(State_2& map2, std::vector<ccmpl::ColorAt>& dot){
  dot.clear();
  ccmpl::RGB col = getcolor(map2.x, 0, 1);
  dot.push_back({{1.1,1.1}, col});
}


void fill_bmu(State_1& map1, ccmpl::Point& dot){
  dot = {map1.tw(map1.bmu).x, map1.tw(map1.bmu).y};
}

void fill_bmu_2(State_2& map2, ccmpl::Point& dot){
  dot = {map2.bmu.x, map2.bmu.y};
}
void fill_colorbar(std::vector<ccmpl::ColorAt>& colored_dots){
  colored_dots.clear();
  for(double i = 0 ; i< 1.; i+=0.005){
  //  ccmpl::RGB col(i,i*i,(1-i)*(1-i));
    ccmpl::RGB col = getcolor(i, 0,1);
    colored_dots.push_back({{0,i},col});
  }
}

void fill_maps( State_1& map1, State_2& map2, std::vector<ccmpl::ColorAt>& colored_dots ){
  colored_dots.clear();
  for(int i = 0; i < MAP_SIZE ; i++){
    for(int j = 0; j< MAP_SIZE ; j++){
        xsom::Point2D pos = map1.cw.mapping.index2pos({i,j});
        xsom::Point2D p = map1.cw(pos);
        xsom::Point2D p1 = map1.tw(pos);
        ccmpl::RGB col = getcolor(map2.tw(pos), 0, 1);
        colored_dots.push_back({{p1.x, p1.y},col});
      }
  }
}

ccmpl::RGB getcolor(double v,double vmin,double vmax)
{
   ccmpl::RGB c(v,v*v,(1-v)*(1-v)); // white
/*   double dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {
      c.r = 0;
      c.g = 4 * (v - vmin) / dv;
   } else if (v < (vmin + 0.5 * dv)) {
      c.r = 0;
      c.b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      c.r = 4 * (v - vmin - 0.5 * dv) / dv;
      c.b = 0;
   } else {
      c.g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      c.b = 0;
   }*/

   return(c);
}


//ccmpl::RGB color_of(const double& p) {return {p, p*p, (1-p)*(1-p)};}
ccmpl::RGB color_of(const double& p) {return getcolor(p, 0,1);}

void plot_positions(std::string& flags,ccmpl::chart::Layout& display,
  State_1& map1, State_2& map2, Results& results){

  display().title = "Map representation in input space";
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::lines("'r'",         std::bind(xsom::tab2d::fill_lines<NB_W, NB_H, STEP>, std::ref(map1.tw), _1));    flags += "#";
  display()  += ccmpl::dot("color='k', s = 100", std::bind(fill_input, std::ref(map1), _1)); flags += "#";
  display()  += ccmpl::dot("color='b', s = 100", std::bind(fill_bmu, std::ref(map1), _1)); flags += "#";
  display++;

  display().title = "Map 1 cortical representation in positions space";
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::lines("'r'",         std::bind(xsom::tab2d::fill_lines<NB_W, NB_H, STEP>, std::ref(map1.cw), _1));                                    flags += "#";

  display++;

  display().title = "Map 2 Weigths";
  display().xtitle  = "Position in map";
  display()   = ccmpl::view2d({0.,1.}, {0.,1.}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  +=ccmpl::image("interpolation='bilinear'",
                [&map2](std::vector<double>& x,
                        std::vector<double>& y,
                        std::vector<double>& z,
                        unsigned int& width,
                        unsigned int& depth)
                        {
                          map2.tw.fill_image_rgb(color_of, x, y, z, width, depth);});            flags += "#";

  display()  += ccmpl::dot("color='c', s = 100", std::bind(fill_bmu_2, std::ref(map2), _1)); flags += "#";

  display++;

  display().title = "Map 2 cortical representation in positions space";
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::lines("'r'",         std::bind(xsom::tab2d::fill_lines<NB_W, NB_H, STEP>, std::ref(map2.cw), _1));                                    flags += "#";

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

  display++;

  display().title = "map2(map1)";
  display()   = ccmpl::view2d({0.,1.}, {0.,1.}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()  += ccmpl::confetti("s=80",         std::bind(fill_maps, std::ref(map1), std::ref(map2), _1));                             flags += "#";

}

void plot_activity(std::string& flags,ccmpl::chart::Layout& display,
  State_1& map1, State_2& map2, Results& results){

  display().title = "Thalamic weights 1 representation in input space";
  display().xtitle  = "x";
  display().ytitle  = "y";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::lines("'r'",         std::bind(xsom::tab2d::fill_lines<NB_W, NB_H, STEP>, std::ref(map1.tw), _1));    flags += "#";
  display()  += ccmpl::dot("color='k', s = 100", std::bind(fill_input, std::ref(map1), _1)); flags += "#";
  display()  += ccmpl::dot("color='b', s = 100", std::bind(fill_bmu, std::ref(map1), _1)); flags += "#";
  display++;

  display().title = "Map 2 Weigths";
  display().xtitle  = "Position in map";
  display()   = ccmpl::view2d({0.,1.2}, {0.,1.2}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  +=ccmpl::image("interpolation='bilinear'",
                [&map2](std::vector<double>& x,
                        std::vector<double>& y,
                        std::vector<double>& z,
                        unsigned int& width,
                        unsigned int& depth)
                        {
                          map2.tw.fill_image_rgb(color_of, x, y, z, width, depth);});            flags += "#";

  display()  += ccmpl::dot("color='c', s = 100", std::bind(fill_bmu_2, std::ref(map2), _1)); flags += "#";
  display()  += ccmpl::confetti("s = 150", std::bind(fill_input_2, std::ref(map2), _1)); flags += "#";
  display++;


  display().title = "Cortical activity map1";
  display().xtitle  = "axe 1";
  display().ytitle  = "axe 2";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::image("cmap= 'inferno', interpolation='bilinear'" ,std::bind(&xsom::tab2d::Table<double>::fill_image_gray, std::ref(map1.ca),_1,_2,_3,_4,_5) ); flags += "#";

  display++;
  display().title = "Cortical activity map2";
  display().xtitle  = "axe 1";
  display().ytitle  = "axe 2";
  display()   = ccmpl::view2d({0., 1.}, {0., 1.1}, ccmpl::aspect::fit, ccmpl::span::placeholder);
  display()  += ccmpl::image("cmap= 'inferno', interpolation='bilinear'",std::bind(&xsom::tab2d::Table<double>::fill_image_gray, std::ref(map2.ca),_1,_2,_3,_4,_5)); flags += "#";

  }
