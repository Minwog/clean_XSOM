#include "helpers.hpp"

void compute_mean_error(Results& results, int iteration){
  auto w_1 = results.bmu_w1_learning.begin()+ iteration - 50; //first it is 1
  auto w_2 = results.bmu_w2_learning.begin()+ iteration - 50;
  auto in_1 = results.inputsx.begin()+ iteration - 50;
  auto in_2 = results.inputsy.begin()+ iteration - 50;
  Pos err_moy = 0;
  for(int i = 0; i<50; i++){
    Pos dist = (*w_1 - *in_1)*(*w_1 - *in_1) + (*w_2 - *in_2)*(*w_2 - *in_2);
    err_moy = err_moy + dist;
    w_1 ++ ; in_1 ++; w_2 ++; in_2 ++;
  }

  err_moy = std::sqrt(err_moy)/ 50 ;
  results.mean_error_batch.push_back(err_moy);
}

void compute_test_error(Results& results){
  auto w_1 = results.bmu_w1.begin(); //first it is 1
  auto w_2 = results.bmu_w2.begin();
  auto in_1 = results.inputsx.begin();
  auto in_2 = results.inputsy.begin();
  Pos err_moy = 0;
  for(int i = 0; i<results.bmu_w1.size(); i++){
    Pos dist = (*w_1 - *in_1)*(*w_1 - *in_1) + (*w_2 - *in_2)*(*w_2 - *in_2);
    err_moy = err_moy + dist;
    w_1 ++ ; in_1 ++; w_2 ++; in_2 ++;
  }

  auto clw_1 = results.bmu_w1_closed.begin(); //first it is 1
  auto clw_2 = results.bmu_w2_closed.begin();
  auto clin_1 = results.inputsx_closed.begin();
  auto clin_2 = results.inputsy_closed.begin();
  Pos err_moy2 = 0;
  for(int i = 0; i<results.bmu_w1_closed.size(); i++){
    Pos dist = (*w_1 - *in_1)*(*w_1 - *in_1) + (*w_2 - *in_2)*(*w_2 - *in_2);
    err_moy2 = err_moy2 + dist;
    clw_1 ++ ; clin_1 ++; clw_2 ++; clin_2 ++;
  }

  err_moy2 = std::sqrt(err_moy2)/ results.bmu_w1_closed.size();
  results.mean_error = err_moy;
  results.mean_error_closed = err_moy2;
}

//plot poids des bmus au cours du temps, erreurs, position des bmus,
void save_res_csv(Results& results,std::string& file_name){
  std::ofstream f(file_name.c_str());
  for(auto& p1:results.inputsx) f<<p1<<";";
  f<<"\n";
  for(auto& p2:results.inputsy) f<<p2<<";";
  f<<"\n";
  for(auto& p1:results.inputsx_closed) f<<p1<<";";
  f<<"\n";
  for(auto& p2:results.inputsy_closed) f<<p2<<";";
  f<<"\n";
  for(auto& p1:results.inputsx_learn) f<<p1<<";";
  f<<"\n";
  for(auto& p2:results.inputsy_learn) f<<p2<<";";
  f<<"\n";
  for(auto& p3:results.bmu_w1_learning) f<<p3<<";";
  f<<"\n";
  for(auto& p4:results.bmu_w2_learning) f<<p4<<";";
  f<<"\n";
  for(auto& p5:results.bmu_w1) f<<p5<<";";
  f<<"\n";
  for(auto& p6:results.bmu_w2) f<<p6<<";";
  f<<"\n";
  for(auto& p7:results.bmu_w1_closed) f<<p7<<";";
  f<<"\n";
  for(auto& p8:results.bmu_w2_closed) f<<p8<<";";
  f<<"\n";
  for(auto& p9:results.bmu_1) f<<p9<<";";
  f<<"\n";
  for(auto& p10:results.bmu_2) f<<p10<<";";
  f<<"\n";
  for(auto& p11:results.mean_error_batch) f<<p11<<";";
}
