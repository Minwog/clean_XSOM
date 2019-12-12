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
#define MAP_SIZE 100
#define T_MATCH_SIGMA .5
#define ALPHA_MIN .05
#define ALPHA_MAX .3
#define ALPHA_OUT .1
#define H_RADIUS_MAX .1
#define H_RADIUS_MIN 0.02
#define KERNEL_TYPE xsom::tab::fft::KernelType::Gaussian  // We use a Gaussian convolution kernel
#define SIGMA_CONV .005                               // This is its standard deviation un Pos units.
#define GRID_SIGMA (SIGMA_CONV * MAP_SIZE)                // This is its standard deviation in table index units (since MAP_SIZE is mapped into [0,1]).
#define T_MATCH_SIGMA_2 ((T_MATCH_SIGMA)*(T_MATCH_SIGMA))
#define MEM_SIZE 500
#define N_IT 1000


using Pos      = xsom::Point2D<double> ; // The maps are 1D
using Input    = xsom::Point2D<double> ; // Inputs are 2D values.
using TWeight  = Input;  // "Thalamic" weights (i.e weights matching the input).
using Act      = double; // The map activities are scalar.
using Acts     = xsom::tab2d::Table<Act>;
using TWeights = xsom::tab2d::Table<TWeight>;
using Map      = xsom::tab2d::fft::Convolution;

struct Results{
  std::vector<Input> inputsx;
  std::vector<Input> bmu_w1;
};


class Params{
public:

  double t_match_sigma_2 = T_MATCH_SIGMA_2;
  double h_radius = H_RADIUS_MAX;

  double _alpha(int& iteration) const { return ALPHA_MAX - iteration * (ALPHA_MAX - ALPHA_MIN)/N_IT; };
  //double _alpha(int& iteration) const { if (iteration < N_IT/2) return ALPHA_MAX; else return ALPHA_MIN; };
  double _t_match_sigma_2() const { return T_MATCH_SIGMA_2;};
  double _h_radius(int& iteration) const{ return H_RADIUS_MAX - iteration * (H_RADIUS_MAX-H_RADIUS_MIN)/N_IT;};


};


static void update_output( Pos& out, const Pos& request) {
  out = request;
 }

 double t_dist(const TWeight& w, const Input& xi);

 double t_match(const Params& params, const Pos& local, const TWeight& w, const Input& x) ;

 double m_dist(const Pos& w1,const Pos& w2);

 double h( const Params& params, const Pos& bmu,const Pos& p);

 double merge_simple( const Acts& thal,const Pos& p);

class State{
public:
  Input       x     = {0,0};        // The current input
  Pos         bmu   = {0,0};        // The bmu position
  xsom::tab2d::Mapping mapping; // converts positions to table index.
  Acts        ta;               // thalamic matching
  TWeights    tw;               // thalamic weights
  Map         ma;               // map activities
  int mem_size  ;               //size of deques for memory

  std::deque<Pos> bmus;

//  template<typename RANDOM_DEVICE>
//  State(RANDOM_DEVICE& rd);




template<typename RANDOM_DEVICE>
State(RANDOM_DEVICE& rd)
  : mapping({0.,0.}, {1.,1.}, {MAP_SIZE,MAP_SIZE}),
    ta(mapping),
    tw(mapping),
    ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

  // Random initialization.
  auto uniform = std::uniform_real_distribution<double>(0,1);
  for(auto& c : tw.content) {c.x=uniform(rd) ; c.y = uniform(rd);}
}

//  template<typename RANDOM_DEVICE>
  //State(RANDOM_DEVICE& rd, const State& map);


  template<typename RANDOM_DEVICE>
  void reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) {c.x=uniform(rd) ; c.y = uniform(rd);}
  }


  void next(const Input& o) ;

  void save(const std::string& filename, bool mode_append) const ;

  void save_act(const std::string& filename, bool mode_append) const ;

  void load(const std::string& filename) ;



/*  template<typename RANDOM_DEVICE>
  void reinit(RANDOM_DEVICE& rd);*/
};
