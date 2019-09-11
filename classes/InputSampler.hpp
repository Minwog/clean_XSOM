#pragma once

#include <xsom.hpp>
#include <ccmpl.hpp>
#include <random>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include <vector>
#include <utility>

#define PI 3.141592

//A changer : utiliser un vecteur de tableaux, avec template pour taille du tableau. Ou alors créer une classe Point.

using Point = std::pair<double,double>;
using Colormap = std::pair<xsom::Point2D<double>, double>;

template<typename INPUT>
class InputSampler{

protected:
  std::vector<INPUT> values;
  std::vector<INPUT> test_values;
  typename std::vector<INPUT>::iterator current;
  typename std::vector<INPUT>::iterator current_test;

  std::string test_file;
  std::string file;
  bool generate_new_file = false;


  public:

  int size = 0;
  int size_test = 0;
  bool test_mode = false;
  const int dimension;

  InputSampler(void) = default;

  InputSampler(int taille, int taille_test,int dim, bool new_file):
    size(taille),
    size_test(taille_test),
    dimension(dim),
    generate_new_file(new_file)
    {
    }

  InputSampler(const InputSampler&) = default;

  ~InputSampler(void){};

  void operator()(){
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(1,size);
    int random_number = uni(rd);
    if(!test_mode){
    for(int i=0;i<random_number;i++){
      if(++current==values.end()) current = values.begin();
    }
  }else{
    if(++current_test == test_values.end()) current_test = test_values.begin();
    }
  };


  void save_input_file(const std::string& file_name){

  }; //no function for the moment

  bool read_input_file(const std::string& file_name){ //no function for the moment.
    return false;
  };



  virtual void generate_test(std::random_device& rd) = 0;

  template<typename RANDOM_DEVICE>
  void generate_inputs(RANDOM_DEVICE rd){
    //empty definition for templates
  }

};


class Random_Circle_2D: public InputSampler<Point> {

public:
  double r;
  double center_x;
  double center_y;

   Random_Circle_2D(int taille, std::string filename, int taille_test,  std::string test_filename,double x, double y, double r):
   InputSampler(taille, taille_test,2,true),
   center_x(x),
   center_y(y),
   r(r){
       std::random_device rd;
       std::mt19937 gen(rd());
       generate_inputs(gen);
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }

  ~Random_Circle_2D(){};

  template<typename RANDOM_DEVICE>
  void generate_inputs(RANDOM_DEVICE rd){
    auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
    for(int i=0;i<size;i++){
        double res1 = uniform(rd);
        Point tmp(center_x+r*std::cos(res1),center_y+r*std::sin(res1));
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   for(int i=0;i<size_test;i++){
     double res1 = uniform(rd);
     Point tmp(center_x+r*std::cos(res1),center_y+r*std::sin(res1));
       test_values.push_back(tmp);
     }
 };

 Point get(){
 if(!test_mode){
   return (*current);
 }else{
  return (*current_test);
 }
}

//Save current input

void save(std::ofstream& file){
    if(!test_mode){
        file<<(*current).first<<","<<(*current).second;
        file<<'\n';
    }else{
      file<<(*current_test).first<<","<<(*current_test).second;
      file<<'\n';
    }
}

};

class Random_Colormap: public InputSampler<Colormap> {

public:
   std::array<std::array<double, 5>,4> colortab { {{0.2,0.4,0.6,0.8,1.},{0.2,0.2,0.8,0.4,0.2},{1.,0.6,0.4,1.,0.6},{0.6,0.8,0.6,0.4,0.8}} };

   Random_Colormap(int taille, std::string filename, int taille_test,  std::string test_filename):
   InputSampler(taille, taille_test,2,true)
   {
       std::random_device rd;
       std::mt19937 gen(rd());
       generate_inputs(gen);
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();

   }

  ~Random_Colormap(){};

  template<typename RANDOM_DEVICE>
  void generate_inputs(RANDOM_DEVICE rd){
    auto uniform = std::uniform_real_distribution<double>(0., 1.);
    for(int i=0;i<size;i++){
      xsom::Point2D tmp(uniform(rd), uniform(rd));
      double col = point_to_color(tmp);
      Colormap elt(tmp, col);
      values.push_back(elt);
      }

  }

 void generate_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   for(int i=0;i<size_test;i++){
     auto uniform = std::uniform_real_distribution<double>(0., 1.);
       xsom::Point2D tmp(uniform(rd), uniform(rd));
       double col = point_to_color(tmp);
       Colormap elt(tmp, col);
       test_values.push_back(elt);
     }
   }

 Colormap get(){
 if(!test_mode){
   return (*current);
 }else{
  return (*current_test);
 }
}

//Save current input

  void save(std::ofstream& file){
      if(!test_mode){
          file<<(*current).first.x<<","<<(*current).first.y<<","<<(*current).second;
          file<<'\n';
      }else{
        file<<(*current_test).first.x<<","<<(*current_test).first.y<<","<<(*current_test).second;
        file<<'\n';
      }
  }

  double point_to_color(xsom::Point2D<double>& pt){
    int idx1 = int(std::floor(pt.x*5));
    int idx2 = int(std::floor(pt.y*4));
    return colortab[idx2][idx1];
  }
};




//tirer au hasard un point dans une collection : existe dans la stl.
//
