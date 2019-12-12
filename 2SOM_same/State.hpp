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
#define MAP_SIZE 500
#define T_MATCH_SIGMA .5
#define C_MATCH_SIGMA .5
#define ALPHA_MIN .05
#define BETA 0.5
#define ALPHA_MAX .3
#define ALPHA_OUT .1
#define H_RADIUS_MAX .1
#define H_RADIUS_MIN 0.02
#define KERNEL_TYPE xsom::tab::fft::KernelType::Gaussian  // We use a Gaussian convolution kernel
#define SIGMA_CONV .005                               // This is its standard deviation un Pos units.
#define GRID_SIGMA (SIGMA_CONV * MAP_SIZE)                // This is its standard deviation in table index units (since MAP_SIZE is mapped into [0,1]).
#define T_MATCH_SIGMA_2 ((T_MATCH_SIGMA)*(T_MATCH_SIGMA))
#define C_MATCH_SIGMA_2 ((C_MATCH_SIGMA)*(C_MATCH_SIGMA))
#define MEM_SIZE 500
#define N_IT 1000


using Pos      = double ; // The maps are 1D
using Input    = xsom::Point2D<double> ; // Inputs are 2D values.
using TWeight  = Input;  // "Thalamic" weights (i.e weights matching the input).
using CWeight = Pos;
using Act      = double; // The map activities are scalar.
using Acts     = xsom::tab1d::Table<Act>;
using TWeights = xsom::tab1d::Table<TWeight>;
using CWeights = xsom::tab1d::Table<CWeight>;
using Map      = xsom::tab1d::fft::Convolution;

struct Results{
  std::vector<Input> inputsx;
  std::vector<Input> bmu_w1;
  std::vector<Input> bmu_w2;
};


class Params{
public:

  double t_match_sigma_2 = T_MATCH_SIGMA_2;
  double h_radius = H_RADIUS_MAX;
  double c_match_sigma_2 = C_MATCH_SIGMA_2;
  double _alpha(int& iteration) const { return ALPHA_MAX - iteration * (ALPHA_MAX - ALPHA_MIN)/N_IT; };
  //double _alpha(int& iteration) const { if (iteration < N_IT/2) return ALPHA_MAX; else return ALPHA_MIN; };
  double _t_match_sigma_2() const { return T_MATCH_SIGMA_2;};
  double _h_radius(int& iteration) const{ return H_RADIUS_MAX - iteration * (H_RADIUS_MAX-H_RADIUS_MIN)/N_IT;};


};


static void update_output( Pos& out, const Pos& request) {
  out = request;
 }

 static void update_output_vit_prop( Pos& out, const Pos& request) {
   Pos diff = (request-out)*ALPHA_OUT;
   out += diff;
  }

 double t_dist(const TWeight& w, const Input& xi);

 double c_dist(const CWeight& w, const Pos& p);

 double t_match(const Params& params, const Pos& local, const TWeight& w, const Input& x) ;

 double c_match(const Params& params, const Pos& local, const CWeight& w, const Pos& p );

 double m_dist(const Pos& w1,const Pos& w2);

 double h( const Params& params, const Pos& bmu,const Pos& p);

 double merge(const float& beta, const Acts& thal, const Acts& cort, const Pos& p);


class State{
public:
  Input       x     = {0,0};        // The current input
  Pos         bmu   = 0;        // The bmu position
  xsom::tab1d::Mapping mapping; // converts positions to table index.
  Acts        ta;               // thalamic matching
  TWeights    tw;               // thalamic weights
  Acts        ca;               // cortical matching
  CWeights    cw;               // cortical weights
  Map         ma;               // map activities
  int mem_size  ;               //size of deques for memory
  double beta = BETA;
  std::deque<Pos> bmus;

//  template<typename RANDOM_DEVICE>
//  State(RANDOM_DEVICE& rd);




template<typename RANDOM_DEVICE>
State(RANDOM_DEVICE& rd)
  : mapping(0, 1, MAP_SIZE),
    ta(mapping),
    tw(mapping),
    ca(mapping),
    cw(mapping),
    ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

  // Random initialization.
  auto uniform = std::uniform_real_distribution<double>(0,1);
  for(auto& c : tw.content) {c.x=uniform(rd) ; c.y = uniform(rd);}
  for(auto& c : cw.content)  c = uniform(rd);
}



  template<typename RANDOM_DEVICE>
  void reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) {c.x=uniform(rd) ; c.y = uniform(rd);}
    for(auto& c : cw.content)  c = uniform(rd);
  }


  void next(const Input& o) ;

  void save(const std::string& filename, bool mode_append) const ;

  void save_act(const std::string& filename, bool mode_append) const ;

  void load(const std::string& filename) ;

  void close_thalamic() ;

  void open_thalamic() ;

  void set_beta(float b) ;

  void reinit_bmu(Pos p);

  Pos compute_bmu_thal();


/*  template<typename RANDOM_DEVICE>
  void reinit(RANDOM_DEVICE& rd);*/
};
