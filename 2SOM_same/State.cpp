#include "State.hpp"


//Program parameters


//We're using euclidian distance on inputs and pos.
template<typename X>
double euclidian(const X& x1,const X& x2) {
  X d = x1-x2;
  return d*d;
}

double t_dist(const TWeight& w, const Input& xi) {return euclidian(w,xi);}

double c_dist(const CWeight& w, const Pos& p) {return euclidian(w,p);}

double t_match( const Params& params, const Pos& local, const TWeight& w, const Input& x) {return xsom::gaussian(w,x,params.t_match_sigma_2,t_dist);}
//double t_match( const Params& params, const Pos& local, const TWeight& w, const Input& x) {return xsom::gaussian(w,x,T_MATCH_SIGMA_2,t_dist);}

double c_match(const Params& params, const Pos& local, const CWeight& w, const Pos& p) {return xsom::gaussian(w,p,params.c_match_sigma_2,c_dist);}


double m_dist(const Pos& w1,const Pos& w2) {return euclidian(w1,w2);}

double h(const Params& params, const Pos& bmu,const Pos& p ) {return xsom::linear(p, bmu, params.h_radius, m_dist);}
//double h(const Params& params, const Pos& bmu,const Pos& p ) {return xsom::linear(p, bmu, H_RADIUS, m_dist);}


double merge(const float& beta, const Acts& thal, const Acts& cort, const Pos& p) {
  double t = thal(p);
  double c = cort(p);
  return beta * t + (1 - beta) * c;

}


  void State::next(const Input& o) {
    // This is for next step computation.
    x  = o;
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
