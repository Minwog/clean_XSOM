#include "2D_state.hpp"


//Program parameters


//We're using euclidian distance on inputs and pos.
template<typename X>
double euclidian(const X& x1,const X& x2) {
  X d = x1-x2;
  return d*d;
}

//We're using same distances and parameters on each map for now
double t_dist_1(const TWeight1& w, const Input1& xi) {return euclidian(w,xi);}

double t_dist_2(const TWeight2& w, const Input2& xi) {return euclidian(w,xi);}

double c_dist(const CWeight& w, const Pos& p)  {return euclidian(w,p);}

double t_match_1(const Pos& local, const TWeight1& w, const Input1& x) {return xsom::gaussian(w,x,T_MATCH_SIGMA_2,t_dist_1);}

double t_match_2(const Pos& local, const TWeight2& w, const Input2& x) {return xsom::gaussian(w,x,T_MATCH_SIGMA_2,t_dist_2);}

double c_match(const Pos& local, const CWeight& w, const Pos& p) {return xsom::gaussian(w,p,C_MATCH_SIGMA_2,c_dist);}

double m_dist(const Pos& w1,const Pos& w2) {return euclidian(w1,w2);}

double h(const Pos& bmu,const Pos& p) {return xsom::linear(p, bmu, H_RADIUS, m_dist);}

double merge(const float& beta, const Acts& thal, const Acts& cort, const Pos& p) {
  double t = thal(p);
  double c = cort(p);
  return beta * t + (1 - beta) * c;
}


double merge_simple(const Acts& thal, const Pos& p) {
  double t = thal(p);
  return t;
}



  void State_1::next(const Input1& o) {
    // This is for next step computation.
    x  = o;
    bmus.push_back(bmu);
    if(bmus.size()>MEM_SIZE) bmus.pop_front();
  }

  void State_1::save(const std::string& filename, bool mode_append) const {
      if(mode_append){
        std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
        f << tw << cw <<"\n";
      }else{
        std::ofstream f(filename.c_str());
        f << tw << cw ;
    }
  }

  void State_1::save_act(const std::string& filename, bool mode_append) const {
    if(mode_append){
      std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
      f << ta << ca << "\n";
  }else{
      std::ofstream f(filename.c_str());
      f << ta << ca ;
  }
  }

  void State_1::load(const std::string& filename) {
    std::ifstream f(filename.c_str());
    f >> tw >> cw;
  }

  void State_1::close_thalamic(){
    if(beta > 0.) beta = 0;
  }

  void State_1::open_thalamic(){
    if(beta == 0.) beta = BETA;
  }

  void State_1::set_beta(float b){
    beta = b;
  }

  void State_1::reinit_bmu(Pos p){
    bmu=p;
  }

  Pos State_1::compute_bmu_thal(){
    return ta.bmu();
  }


  void State_2::next(const Input2& o) {
    // This is for next step computation.
    x  = o;
    bmus.push_back(bmu);
    if(bmus.size()>MEM_SIZE) bmus.pop_front();
  }

  void State_2::save(const std::string& filename, bool mode_append) const {
      if(mode_append){
        std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
        f << tw << cw <<"\n";
      }else{
        std::ofstream f(filename.c_str());
        f << tw << cw ;
    }
  }

  void State_2::save_act(const std::string& filename, bool mode_append) const {
    if(mode_append){
      std::ofstream f(filename.c_str(),std::ios_base::app | std::ios_base::out);
      f << ta << ca << "\n";
  }else{
      std::ofstream f(filename.c_str());
      f << ta << ca ;
  }
  }

  void State_2::load(const std::string& filename) {
    std::ifstream f(filename.c_str());
    f >> tw >> cw;
  }

  void State_2::close_thalamic(){
    if(beta > 0.) beta = 0;
  }

  void State_2::open_thalamic(){
    if(beta == 0.) beta = BETA;
  }

  void State_2::set_beta(float b){
    beta = b;
  }

  void State_2::reinit_bmu(Pos p){
    bmu=p;
  }

  Pos State_2::compute_bmu_thal(){
    return ta.bmu();
  }


  /*template<typename RANDOM_DEVICE>
  void State_1::reinit(RANDOM_DEVICE& rd){
    auto uniform = std::uniform_real_distribution<double>(0, 1);
    for(auto& c : tw.content) c = uniform(rd);
  }*/
