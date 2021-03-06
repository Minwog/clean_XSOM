#include "Simple_state.hpp"


//Program parameters


//We're using euclidian distance on inputs and pos.
template<typename X>
double euclidian(const X& x1,const X& x2) {
  X d = x1-x2;
  return d*d;
}

//We're using same distances and parameters on each map for now
double t_dist(const TWeight& w, const Input& xi) {return euclidian(w,xi);}

double c_dist(const CWeight& w, const Input& p)  {return euclidian(w,p);}

double t_match(const Pos& local, const TWeight& w, const Input& x) {return xsom::gaussian(w,x,T_MATCH_SIGMA_2,t_dist);}

double c_match(const Pos& local, const CWeight& w, const Pos& p) {return xsom::gaussian(w,p,C_MATCH_SIGMA_2,c_dist);}

double m_dist(const Pos& w1,const Pos& w2) {return euclidian(w1,w2);}

double h(const Params& params, const Pos& bmu,const Pos& p) {return xsom::linear(p, bmu, params.h_radius, m_dist);}

double merge(const float& beta, const Acts& thal, const Acts& cort, const Pos& p) {
  double t = thal(p);
  double c = cort(p);
  return beta * t + (1 - beta) * c;
}




// This updates the output position of the map from a position request
// (which is the BMU).

//putting update_output in each state, so we can trigger effects on states.


/*  template<typename RANDOM_DEVICE>
  State::State(RANDOM_DEVICE& rd)
    : mapping(0, 1, MAP_SIZE),
      ta(mapping),
      tw(mapping),
      ca(mapping),
      cw(mapping),
      ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

    // Random initialization.
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) c = uniform(rd);
    for(auto& c : cw.content) c = uniform(rd);


  }*/

  void State::next(const Input& o) {
    // This is for next step computation.
    x  = o;
    _x = bmu;
    if(bmus.size()>MEM_SIZE) bmus.pop_front();
  }

  void State::save(const std::string& filename, bool mode_append) const {
      if(mode_append){
        std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
        f << tw << cw <<"\n";
      }else{
        std::ofstream f(filename.c_str());
        f << tw << cw ;
    }
  }

  void State::save_act(const std::string& filename, bool mode_append) const {
    if(mode_append){
      std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
      f << ta << ca << "\n";
  }else{
      std::ofstream f(filename.c_str());
      f << ta << ca ;
  }
  }

  void State::load(const std::string& filename) {
    std::ifstream f(filename.c_str());
    f >> tw >> cw;
  }

  void State::close_thalamic(){
    if(beta > 0.) beta = 0;
  }

  void State::open_thalamic(){
    if(beta == 0.) beta = BETA;
  }

  void State::set_beta(float b){
    beta = b;
  }

  void State::reinit_bmu(Pos p){
    bmu=p;
  }

  Pos State::compute_bmu_thal(){
    return ta.bmu();
  }

/*  template<typename RANDOM_DEVICE>
  void State::reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) c = uniform(rd);
    for(auto& c : cw.content) c = uniform(rd);
  }

*/
  /*template<typename RANDOM_DEVICE>
  State_1::State_1(RANDOM_DEVICE& rd)
    : mapping(0, 1, MAP_SIZE),
      ta(mapping),
      tw(mapping),
      ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

    // Random initialization.
    auto uniform = std::uniform_real_distribution<double>(-0.5,0.5);
    for(auto& c : tw.content) c = uniform(rd);
  }*/
/*  template<typename RANDOM_DEVICE>
  State_1::State_1(RANDOM_DEVICE& rd, const State& map)
    : mapping(map.mapping),
      ta(mapping),
      tw(mapping),
      ma(mapping, GRID_SIGMA, KERNEL_TYPE ) {

    // Random initialization.
    int count = 0;
    for(auto& c : tw.content) c = map.tw.content[count++];
  }*/

  void State_1::next(const Input& o) {
    // This is for next step computation.
    x  = o;
    bmus.push_back(bmu);
    if(bmus.size()>MEM_SIZE) bmus.pop_front();
  }

  void State_1::save(const std::string& filename, bool mode_append) const {
  if(mode_append){
      std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
      f << tw << "\n";
  }else{
      std::ofstream f(filename.c_str());
      f << tw  ;
  }
  }

    void State_1::save_act(const std::string& filename, bool mode_append) const {
    if(mode_append){
      std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
      f << ta << "\n";
  }else{
      std::ofstream f(filename.c_str());
      f << ta  ;
  }
  }

  void State_1::load(const std::string& filename) {
    std::ifstream f(filename.c_str());
    f >> tw;
  }

  /*template<typename RANDOM_DEVICE>
  void State_1::reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) c = uniform(rd);
  }*/
