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
using Point3D = std::tuple<double,double,double>;

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

  virtual void generate_noisy_test(std::random_device& rd) = 0;

  virtual void generate_inputs(std::random_device& rd) = 0;

};

//template<typename RANDOM_DEVICE>
class Random_Circle_2D: public InputSampler<Point> {

public:
  double r;
  double center_x;
  double center_y;

   Random_Circle_2D(int taille, std::string filename, int taille_test, std::string test_filename,double x, double y, double r):
   InputSampler(taille, taille_test,2,true),
   center_x(x),
   center_y(y),
   r(r){
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }


      Random_Circle_2D(int taille, std::string filename, int taille_test, std::string test_filename,double x, double y, double r,bool noisy):
      InputSampler(taille, taille_test,2,true),
      center_x(x),
      center_y(y),
      r(r){
          std::random_device rd;
          generate_inputs(std::ref(rd));
          if(noisy) generate_noisy_test(std::ref(rd));
          else generate_test(std::ref(rd));
          current=values.begin();
          current_test=test_values.begin();
      }

  ~Random_Circle_2D(){};

  void generate_inputs(std::random_device& rd) override {
    std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
    for(int i=0;i<size;i++){
      double res1 = uniform(gen);
        Point tmp(center_x+r*std::cos(res1),center_y+r*std::sin(res1));
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
     Point tmp(center_x+r*std::cos(res1),center_y+r*std::sin(res1));
     test_values.push_back(tmp);
     }
 };

 void generate_noisy_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   auto gauss = std::normal_distribution<>{0.01,0.05};
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
     double b1 =gauss(gen);
     double b2 =gauss(gen);
     Point tmp(center_x+r*std::cos(res1)+b1,center_y+r*std::sin(res1)+b2);
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

class Random_Square_2D: public InputSampler<Point> {

public:

   Random_Square_2D(int taille, std::string filename, int taille_test, std::string test_filename):
   InputSampler(taille, taille_test,2,true)
   {
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }


      Random_Square_2D(int taille, std::string filename, int taille_test, std::string test_filename,double x, double y, double r,bool noisy):
      InputSampler(taille, taille_test,2,true)
      {
          std::random_device rd;
          generate_inputs(std::ref(rd));
          if(noisy) generate_noisy_test(std::ref(rd));
          else generate_test(std::ref(rd));
          current=values.begin();
          current_test=test_values.begin();
      }

  ~Random_Square_2D(){};

  void generate_inputs(std::random_device& rd) override {
    std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 1.);
    for(int i=0;i<size;i++){
      double res1 = uniform(gen);
      double res2 = uniform(gen);
        Point tmp(res1,res2);
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 1.);
   for(int i=0;i<size_test;i++){
       double res1 = uniform(gen);
       double res2 = uniform(gen);
        Point tmp(res1,res2);
        test_values.push_back(tmp);
     }
 };

 void generate_noisy_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 1.);
   auto gauss = std::normal_distribution<>{0.01,0.05};
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
     double res2 = uniform(gen);
     Point tmp(res1,res2);
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

class Random_Anneau_2D: public InputSampler<Point> {

public:
  double r;
  double center_x;
  double center_y;
  double largeur;

   Random_Anneau_2D(int taille, std::string filename, int taille_test, std::string test_filename,double x, double y, double r,double l):
   InputSampler(taille, taille_test,2,true),
   center_x(x),
   center_y(y),
   largeur(l),
   r(r){
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }


      Random_Anneau_2D(int taille, std::string filename, int taille_test, std::string test_filename,double x, double y, double r,double l, bool noisy):
      InputSampler(taille, taille_test,2,true),
      center_x(x),
      center_y(y),
      largeur(l),
      r(r){
          std::random_device rd;
          generate_inputs(std::ref(rd));
          if(noisy) generate_noisy_test(std::ref(rd));
          else generate_test(std::ref(rd));
          current=values.begin();
          current_test=test_values.begin();
      }

  ~Random_Anneau_2D(){};

  void generate_inputs(std::random_device& rd) override {
    std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 1.);
    for(int i=0;i<size;i++){
      double dist = -1;
      double res1 = 0;
      double res2 = 0;
      while(dist>r+largeur/2 || dist <r-largeur/2){
        res1 = uniform(gen);
        res2 = uniform(gen);
        dist = std::sqrt((res1-center_x)*(res1-center_x) + (res2-center_y)*(res2-center_y));
      }
        Point tmp(res1,res2);
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 1.);
   for(int i=0;i<size;i++){
     double dist = -1;
     double res1 = 0;
     double res2 = 0;
     while(dist>r+largeur/2 || dist <r-largeur/2){
       res1 = uniform(gen);
       res2 = uniform(gen);
       dist = std::sqrt((res1-center_x)*(res1-center_x) + (res2-center_y)*(res2-center_y));
     }
       Point tmp(res1,res2);
       test_values.push_back(tmp);
     }
 };

 void generate_noisy_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   auto gauss = std::normal_distribution<>{0.01,0.05};
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
     double b1 =gauss(gen);
     double b2 =gauss(gen);
     Point tmp(center_x+r*std::cos(res1)+b1,center_y+r*std::sin(res1)+b2);
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


class Random_spi: public InputSampler<Point> {

public:


   Random_spi(int taille, std::string filename, int taille_test,  std::string test_filename):
   InputSampler(taille, taille_test,2,true)
   {
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }

  ~Random_spi(){};

  void generate_inputs(std::random_device& rd) override {
    std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 4*PI);
    for(int i=0;i<size;i++){
        double res1 = uniform(gen);
        Point tmp(0.5 + 0.04*res1* std::cos(res1),0.5 + 0.04*res1* std::sin(res1) );
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
    std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 4*PI);
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
     Point tmp(0.5 + 0.04*res1* std::cos(res1),0.5 + 0.04*res1* std::sin(res1) );
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



class Random_lissa: public InputSampler<Point> {

public:


   Random_lissa(int taille, std::string filename, int taille_test,  std::string test_filename):
   InputSampler(taille, taille_test,2,true)
   {
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }

  ~Random_lissa(){};

  void generate_inputs(std::random_device& rd) override {
    std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
    for(int i=0;i<size;i++){
        double res1 = uniform(gen);
        Point tmp(0.5 + 0.5* std::sin(2*res1),0.5 + 0.5*std::cos(res1));
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
    std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
      Point tmp(0.5 + 0.5* std::sin(2*res1),0.5 + 0.5*std::cos(res1));
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


class Random_astro: public InputSampler<Point> {

public:


   Random_astro(int taille, std::string filename, int taille_test,  std::string test_filename):
   InputSampler(taille, taille_test,2,true)
   {
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }

  ~Random_astro(){};

  void generate_inputs(std::random_device& rd) override {
    std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
    for(int i=0;i<size;i++){
        double res1 = uniform(gen);
        Point tmp(0.5 + 0.5* std::pow(std::sin(res1),3.0),0.5 + 0.5*std::pow(std::cos(res1),3.));
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
    std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   for(int i=0;i<size_test;i++){
     double res1 = uniform(gen);
      Point tmp(0.5 + 0.5* std::pow(std::sin(res1),3.0),0.5 + 0.5*std::pow(std::cos(res1),3.));
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
     generate_inputs(std::ref(rd));
     generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();

   }

  ~Random_Colormap(){};

  void generate_inputs(std::random_device& rd) override {
     std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 1.);
    for(int i=0;i<size;i++){
      xsom::Point2D tmp(uniform(gen), uniform(gen));
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
       xsom::Point2D tmp(uniform(gen), uniform(gen));
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


class Random_Sphere_3D: public InputSampler<Point3D> {

public:
  double r;
  double center_x;
  double center_y;
  double center_z;

   Random_Sphere_3D(int taille, std::string filename, int taille_test,  std::string test_filename,double x, double y, double z, double r):
   InputSampler(taille, taille_test,2,true),
   center_x(x),
   center_y(y),
   center_z(z),
   r(r){
       std::random_device rd;
       std::mt19937 gen(rd());
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }

  ~Random_Sphere_3D(){};

  void generate_inputs(std::random_device& rd) override {
     std::mt19937 gen(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
    for(int i=0;i<size;i++){
        double res1 = uniform(rd);
        double res2 = uniform(rd);
        Point3D tmp(center_x+r*std::sin(res1)*std::cos(res2),center_y+r*std::sin(res1)*std::sin(res2),center_z + r*std::cos(res1));
        values.push_back(tmp);
      }
  }

 void generate_test(std::random_device& rd) override {
    std::mt19937 gen(rd());
   auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
   for(int i=0;i<size_test;i++){
     double res1 = uniform(rd);
     double res2 = uniform(rd);
     Point3D tmp(center_x+r*std::sin(res1)*std::cos(res2),center_y+r*std::sin(res1)*std::sin(res2),center_z + r*std::cos(res1));
     test_values.push_back(tmp);
     }
 };

 void generate_noisy_test(std::random_device& rd) override {
   std::mt19937 gen(rd());
  auto uniform = std::uniform_real_distribution<double>(0., 2*PI);
  auto gaussian = std::normal_distribution<>{0.05,0.1};
  for(int i=0;i<size_test;i++){
    double res1 = uniform(rd);
    double res2 = uniform(rd);
    Point3D tmp(center_x+r*std::sin(res1)*std::cos(res2) + gaussian(rd),center_y+r*std::sin(res1)*std::sin(res2)+gaussian(rd),center_z + r*std::cos(res1)+gaussian(rd));
    test_values.push_back(tmp);
    }
 };

 Point3D get(){
 if(!test_mode){
   return (*current);
 }else{
  return (*current_test);
 }
}

//Save current input

void save(std::ofstream& file){
    if(!test_mode){
        file<<std::get<0>(*current)<<","<<std::get<1>(*current)<<","<<std::get<2>(*current);
        file<<'\n';
    }else{
      file<<std::get<0>(*current_test)<<","<<std::get<1>(*current_test)<<","<<std::get<2>(*current_test);;
      file<<'\n';
    }
}

};


class Random_1D: public InputSampler<double> {

public:


   Random_1D(int taille, std::string filename, int taille_test,  std::string test_filename):
   InputSampler(taille, taille_test,2,true)
   {
       std::random_device rd;
       generate_inputs(std::ref(rd));
       generate_test(std::ref(rd));
       current=values.begin();
       current_test=test_values.begin();
   }

  ~Random_1D(){};

  void generate_inputs(std::random_device& rd) override {
    values = {0. , 0.1 , 0.2 , 0.3 , 0.2, 0.1 , 0.};
  }

 void generate_test(std::random_device& rd) override {
   test_values = {0. , 0.1 , 0.2 , 0.3 , 0.2, 0.1 , 0.};
 };


  void operator()() {
     if(!test_mode){
       if(++current==values.end()) current = values.begin();
   }else{
     if(++current_test == test_values.end()) current_test = test_values.begin();
     }
   };

 double get(){
 if(!test_mode){
   return (*current);
 }else{
  return (*current_test);
 }
}

//Save current input

void save(std::ofstream& file){
    if(!test_mode){
        file<<(*current);
        file<<'\n';
    }else{
      file<<(*current_test);
      file<<'\n';
    }
}

};




//tirer au hasard un point dans une collection : existe dans la stl.
//
