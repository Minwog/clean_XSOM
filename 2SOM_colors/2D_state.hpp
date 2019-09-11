#pragma once

#include <xsom.hpp>
#include <ccmpl.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <functional>
#include <array>
#include <deque>



using namespace std::placeholders;

//Definitions of all the variables and classes needed for our SOMs
//Can be used for any 1D SOM with double Pos, double Inputs.


//All of the maps have the same parameters.
#define MAP_SIZE 20
#define T_MATCH_SIGMA .5
#define C_MATCH_SIGMA .5
#define ALPHA .1
#define ALPHA_OUT .1
#define H_RADIUS .05
#define BETA 0.5
#define KERNEL_TYPE xsom::tab::fft::KernelType::Gaussian  // We use a Gaussian convolution kernel
#define SIGMA_CONV .005                               // This is its standard deviation un Pos units.
#define GRID_SIGMA (SIGMA_CONV * MAP_SIZE)                // This is its standard deviation in table index units (since MAP_SIZE is mapped into [0,1]).
#define EPSILON 0.002
#define T_MATCH_SIGMA_2 ((T_MATCH_SIGMA)*(T_MATCH_SIGMA))
#define C_MATCH_SIGMA_2 ((C_MATCH_SIGMA)*(C_MATCH_SIGMA))
#define MEM_SIZE 500
#define VIT_OUT 0.002


//Program parameters

//All of the maps have the same type 2D - to change it, define more type, not only input and pos, to use it in variables.

using Pos      = xsom::Point2D<double> ; // The maps are 1D
using CWeight  = Pos;    // "Cortical"  out = request; weights (i.e. weights matching other bmu position).
using Act      = double; // The map activities are scalar.
using Acts     = xsom::tab2d::Table<Act>;
using CWeights = xsom::tab2d::Table<CWeight>;
using Map      = xsom::tab2d::fft::Convolution;

using Input1    = xsom::Point2D<double> ; // Inputs are 2D values.
using TWeight1  = Input1;  // "Thalamic" weights (i.e weights matching the input).
using TWeights1 = xsom::tab2d::Table<TWeight1>;

using Input2   = double; // Inputs are 1D values.
using TWeight2  = Input2;
using TWeights2 = xsom::tab2d::Table<TWeight2>;


struct Results{
  std::vector<Input1> inputsx;
  std::vector<Input2> inputsy;
  std::vector<Input1> bmu_w1_learning;
  std::vector<Input2> bmu_w2_learning;
  std::vector<Input1> bmu_w1;
  std::vector<Input2> bmu_w2;
  std::vector<Input1> bmu_w1_closed;
  std::vector<Input2> bmu_w2_closed;
  std::vector<Pos> bmu_1;
  std::vector<Pos> bmu_2;
  //std::vector<Input> mean_error_batch;
};



double t_dist(const TWeight1& w, const Input1& xi);

double t_dist2(const TWeight2& w, const Input2& xi);

double c_dist(const CWeight& w, const Pos& p);

double t_match_1(const Pos& local, const TWeight1& w, const Input1& x) ;

double t_match_2(const Pos& local, const TWeight2& w, const Input2& x) ;

double c_match(const Pos& local, const CWeight& w, const Pos& p);

double m_dist(const Pos& w1,const Pos& w2) ;

double h(const Pos& bmu,const Pos& p);

double merge(const float& beta, const Acts& thal, const Acts& cort, const Pos& p);


double merge_simple(const Acts& thal, const Pos& p) ;

//putting update_output in each state, so we can trigger effects on states.

/*static void update_output( Pos& out, const Pos& request, std::deque<Pos>& bmu_tab);

static void update_output_vit_const(Pos& out, const Pos& request, std::deque<Pos>& bmu_tab, std::deque<Pos>& diff_tab);*/

static void update_output( Pos& out, const Pos& request, std::deque<Pos>& bmu_tab) {
  out = request;
  bmu_tab.push_back(out);
  if(bmu_tab.size()>MEM_SIZE)  bmu_tab.pop_front();
 }

 /*static void update_output_vit_const(Pos& out, const Pos& request, std::deque<Pos>& bmu_tab, std::deque<Pos>& diff_tab){
   Pos diff = std::min(std::abs(request-out), VIT_OUT);
   if(request-out > 0 ) out += diff;
   if(request-out < 0 ) out -= diff;
   else out = request;

   bmu_tab.push_back(out);
   diff_tab.push_back(diff);
   if(diff_tab.size()>MEM_SIZE) diff_tab.pop_front();
   if(bmu_tab.size()>MEM_SIZE)  bmu_tab.pop_front();

 }*/

 static void update_output_vit_prop( Pos& out, const Pos& request, std::deque<Pos>& bmu_tab, std::deque<Pos>& diff_tab) {
   Pos diff = (request-out)*ALPHA_OUT;
   out += (request-out)*ALPHA_OUT;
   diff_tab.push_back(diff);
   bmu_tab.push_back(out);
   if(diff_tab.size()>MEM_SIZE) diff_tab.pop_front();
   if(bmu_tab.size()>MEM_SIZE)  bmu_tab.pop_front();
  }


class State_1{
public:
  Input1       x    ;        // The current input
  Pos         bmu   = {0,0};        // The bmu position
  float      beta  = BETA;
  xsom::tab2d::Mapping mapping; // converts positions to table index.
  Acts        ta;               // thalamic matching
  TWeights1   tw;               // thalamic weights
  Acts        ca;               // cortical matching
  CWeights    cw;               // cortical weights
  Map         ma;               // map activities

  std::deque<Pos> bmus;
  std::deque<Pos> diff;
//  template<typename RANDOM_DEVICE>
//  State(RANDOM_DEVICE& rd);

  template<typename RANDOM_DEVICE>
  State_1(RANDOM_DEVICE& rd)
    : mapping({0.,0.}, {1.,1.}, {MAP_SIZE,MAP_SIZE}),
      ta(mapping),
      tw(mapping),
      ca(mapping),
      cw(mapping),
      ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

    // Random initialization.
    auto uniform = std::uniform_real_distribution<double>(0., 1.);
    for(auto& c : tw.content){c.x = uniform(rd) ; c.y = uniform(rd);};
    for(auto& c : cw.content) {c.x = uniform(rd) ; c.y = uniform(rd);};
  }

  template<typename RANDOM_DEVICE>
  void reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) {c.x = uniform(rd) ; c.y = uniform(rd);};
    for(auto& c : cw.content) {c.x = uniform(rd) ; c.y = uniform(rd);};
  }

  void next(const Input1& o) ;

  void save(const std::string& filename, bool mode_append ) const ;

  void save_act(const std::string& filename, bool mode_append) const ;

  void load(const std::string& filename) ;

  void close_thalamic() ;

  void open_thalamic() ;

  void set_beta(float b) ;

  void reinit_bmu(Pos p);

  Pos compute_bmu_thal();

};


class State_2{
public:
  Input2       x     = 0;        // The current input
  Pos         bmu   = {0,0};        // The bmu position
  float      beta  = BETA;
  xsom::tab2d::Mapping mapping; // converts positions to table index.
  Acts        ta;               // thalamic matching
  TWeights2    tw;               // thalamic weights
  Acts        ca;               // cortical matching
  CWeights    cw;               // cortical weights
  Map         ma;               // map activities
  int mem_size  ;               //size of deques for memory

  std::deque<Pos> bmus;
  std::deque<Pos> diff;
//  template<typename RANDOM_DEVICE>
//  State(RANDOM_DEVICE& rd);

  template<typename RANDOM_DEVICE>
  State_2(RANDOM_DEVICE& rd)
    : mapping({0.,0.}, {1.,1.}, {MAP_SIZE,MAP_SIZE}),
      ta(mapping),
      tw(mapping),
      ca(mapping),
      cw(mapping),
      ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

    // Random initialization.
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) c = uniform(rd);
    for(auto& c : cw.content){c.x = uniform(rd) ; c.y = uniform(rd);};


  }

  template<typename RANDOM_DEVICE>
  void reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) c = uniform(rd);
    for(auto& c : cw.content) {c.x = uniform(rd) ; c.y = uniform(rd);};
  }

  void next(const Input2& o) ;

  void save(const std::string& filename, bool mode_append ) const ;

  void save_act(const std::string& filename, bool mode_append) const ;

  void load(const std::string& filename) ;

  void close_thalamic() ;

  void open_thalamic() ;

  void set_beta(float b) ;

  void reinit_bmu(Pos p);

  Pos compute_bmu_thal();

};
