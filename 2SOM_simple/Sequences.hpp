#include "../classes/InputSampler.hpp"
#include "Simple_state.hpp"
#include <iostream>
#include <string>
#include <ctime>


#define FIG_PREFIX "../experiences/test_1/test_map"
#define IMG_PREFIX "../experiences/other_shape/map"

const std::string file_prefix = "../experiences/2SOM_simple/";


void compute_mean_error(Results& results, int iteration){
  auto w_1 = results.bmu_w1_learning.begin()+ iteration - 50; //first it is 1
  auto w_2 = results.bmu_w2_learning.begin()+ iteration - 50;
  auto in_1 = results.inputsx.begin()+ iteration - 50;
  auto in_2 = results.inputsy.begin()+ iteration - 50;
  Pos err_moy = 0;
  for(int i = 0; i<50; i++){
    Pos dist = (*w_1 - *in_1)*(*w_1 - *in_1) + (*w_2 - *in_2)*(*w_2 - *in_2);
    err_moy = err_moy + dist;

  }

  err_moy = std::sqrt(err_moy)/ 50 ;
  results.mean_error_batch.push_back(err_moy);
}
//plot poids des bmus au cours du temps, erreurs, position des bmus,
void save_res_csv(Results& results,std::string& file_name){
  std::ofstream f(file_name.c_str());
  for(auto& p1:results.inputsx) f<<p1<<";";
  f<<"\n";
  for(auto& p2:results.inputsy) f<<p2<<";";
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

template<typename INPUTSAMPLER>
void def_sequence(xsom::setup::Sequencer& seq,  State_1& map1,  State& map2,
  bool& testing, INPUTSAMPLER& input_sampler,
  const std::string& flags, const std::string& date, std::ofstream& input_file, std::string& file_prefix, std::ofstream& bmu_file,
  Results& res, std::string& figure_prefix, int& iterations, std::string& file_res,Params& params
){
    seq.interactive(true); // call this after having added menus.

    seq.__def("test_loop");
      seq.__step([&res,&input_file](){
        res.inputsx.clear();
        res.inputsy.clear();

      });

      seq.__for(200);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&res,&input_file](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx.push_back(input_sampler.get().first);
          res.inputsy.push_back(input_sampler.get().second);

              });
        //map1 update, map2 update
        seq.__update();
        seq.__step([&map1,&map2,&bmu_file,&res](){
          bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
          res.bmu_w1.push_back(map1.tw(map1.bmu));
          res.bmu_w2.push_back(map2.tw(map2.bmu));
          res.bmu_1.push_back(map1.bmu);
          res.bmu_2.push_back(map2.bmu);
        });
        seq.__();
      seq.__rof();
      seq.__step([&map2,&seq](){
        map2.close_thalamic();
        seq.msg_info("Test closing map2");
      });
      seq.__for(200);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&res,&input_file](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx.push_back(input_sampler.get().first);
          res.inputsy.push_back(input_sampler.get().second);

        });
          seq.__update();
        seq.__step([&map1,&map2,&bmu_file,&res](){
          res.bmu_w1_closed.push_back(map1.tw(map1.bmu));
          res.bmu_w2_closed.push_back(map2.tw(map2.bmu));
          res.bmu_1.push_back(map1.bmu);
          res.bmu_2.push_back(map2.bmu);
        });

        seq.__();
      seq.__rof();
    seq.__fed();

    seq.__def("learn_batch");
    seq.__step([&seq,&iterations](){
      seq.msg_info(std::to_string(iterations));
    });
    seq.__for(50);
      seq.__step([&map1,&map2,&input_sampler,&res,&input_file](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx.push_back(input_sampler.get().first);
          res.inputsy.push_back(input_sampler.get().second);

              });
      seq.__update();
      seq.__learn();
      seq.__step([&iterations,&params](){
        //res.bmu_w1_learning.push_back(map1.tw(map1.bmu));
        //res.bmu_w2_learning.push_back(map2.tw(map2.bmu));
        iterations++;
        params.h_radius = H_RADIUS_MAX - iterations * (H_RADIUS_MAX-H_RADIUS_MIN)/N_IT;
      });
      seq.__();
    seq.__rof();
    seq.__fed();

    seq.__def("learn_loop");
    seq.__for(20);
      seq.__call("learn_batch");
      /*seq.__step([&res,&iterations](){
        compute_mean_error(std::ref(res), iterations);
      });*/
      seq.__();
    seq.__rof();
    seq.__fed();

  /* */ seq.__loop();
          seq.__call("learn_loop");
          seq.__step([&input_sampler,&seq](){input_sampler.test_mode=true; seq.msg_info("Test phase 1");});
          seq.__call("test_loop");
          seq.__step([&file_res,&res](){
            save_res_csv(std::ref(res), std::ref(file_res));
          });
          seq.__plot_pdf([&flags](){return flags;}, figure_prefix );
	  //seq.__step([&seq](){seq.msg_info("saving image");});
          seq.__step([](){
            throw xsom::instr::Exit();
	    });
  /* */ seq.__pool();
}

/*template<typename INPUTSAMPLER>
void def_sequence_retro(xsom::setup::Sequencer& seq,  State& map1,  State& map2,
  bool& testing, INPUTSAMPLER& input_sampler,
  const std::string& flags, std::vector<double>& inputsx,
  std::vector<double>& inputsy, const std::string& date, std::ofstream& input_file
){

  seq.add_menu_item('t', "t", "switch to test",[&testing,&seq,&map1,&map2,&input_file](){
    testing = true;
    input_file<<"test"<<"\n";
    seq.msg_info("weight saved, Testing mode enabled"); });

    seq.interactive(true); // call this after having added menus.

    seq.__def("test_loop");
      seq.__step([&inputsx,&inputsy](){
        inputsx.clear();
        inputsy.clear();
      });
      seq.__for(1000);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&inputsx,&inputsy](){
          input_sampler();
          map1.next(input_sampler[0]);
          map2.next(input_sampler[1]);
              });
        //map1 update, map2 update
        seq.__for(100);
          seq.__update();
        seq.__rof();
        seq.__plot([&flags](){return flags;});
        //calculate square error on input_space.
        seq.__step([&map1,&map2](){
          double w1 = map1.tw(map1.bmu);
          double w2 = map2.tw(map2.bmu);
        });
      seq.__rof();
      //set Beta to 0 and realise test without map2 thalamic input.
      seq.__step([&map2](){
        map2.close_thalamic();
      });
      //test on all positions
      seq.__step([&inputsx,&inputsy](){
        inputsx.clear();
        inputsy.clear();
      });
      seq.__for(1000);
        seq.__step([&map1,&map2,&input_sampler,&inputsx,&inputsy](){
                input_sampler();
                map1.next(input_sampler[0]);
                map2.next(input_sampler[1]);
                inputsx.push_back(input_sampler[0]);
                inputsy.push_back(input_sampler[1]);

              });
        //map1 update, map2 update
        seq.__for(100);
          seq.__update();
        seq.__rof();
        seq.__plot([&flags](){return flags;});
        //calculate errors
      /*  seq.__step([&pos_err_without_inputs,&map1,&map2,&pos_map1](){
          double pos_map2 = 0.5+0.5*std::sin(pos_map1);
          double w1 = map1.tw(map1.bmu);
          double w2 = map2.tw(map2.bmu);
          double err = (std::abs(pos_map1) - std::abs(w1))*(std::abs(pos_map1) - std::abs(w1))
                      + (std::abs(pos_map2) - std::abs(w2))*(std::abs(pos_map2) - std::abs(w2)); //erreur quadratique.
          pos_err_without_inputs.push_back(err);
        });
*/
      /*  seq.__step([&x_without_in,&y_without_in,&map1,&map2,&step](){
          x_without_in.push_back(map1.tw(map1.bmu));
          y_without_in.push_back(map2.tw(map2.bmu));
        });*/

  /*    seq.__rof();

    seq.__fed();

  /* *//* seq.__loop();
          seq.__for(600);
            seq.__step([&map1,&map2,&input_sampler, &inputsy, &inputsx,&date,&input_file](){
              input_sampler();
              map1.next(input_sampler[0]);
              map2.next(input_sampler[1]);
              inputsx.push_back(input_sampler[0]);
              inputsy.push_back(input_sampler[1]);
              input_sampler.save(input_file);
            });
            seq.__();
          seq.__else();
            seq.__for(100);
              seq.__update();
              seq.__();
            seq.__rof();
            seq.__learn();
            seq.__plot([&flags](){return flags;});
            seq.__step([&map1,&map2,&input_sampler, &inputsy,&inputsx,&input_file,&date](){
              input_sampler();
              map1.next(input_sampler[0]);
              map2.next(input_sampler[1]);
              map1.save(file_prefix + "map1_w_"  + date +".data",true);
              map2.save(file_prefix + "map2_w_"  + date +".data", true);
              inputsx.push_back(input_sampler[0]);
              inputsy.push_back(input_sampler[1]);
              input_sampler.save(input_file);
            });
            seq.__();

          seq.__rof();
          seq.__step([&input_sampler](){input_sampler.test_mode=true;});
          seq.__call("test_step");
  /* */ /*seq.__pool();
}

template<typename INPUTSAMPLER>
void def_sequence_map2(xsom::setup::Sequencer& seq,  State_1& map1,  State& map2, bool& testing, INPUTSAMPLER& input_sampler, const std::string& flags, double& pos_map1){

  seq.add_menu_item('t', "t", "switch to test",[&testing,&seq](){testing = true; seq.msg_info("Testing mode enabled"); });

  seq.interactive(false); // call this after having added menus.
  /* */ /*seq.__loop();
          seq.__if([&testing](){return testing;});
            seq.__step([&input_sampler,&map2](){input_sampler.test_mode = true; map2.set_beta(0);});
            seq.__plot([&flags](){return flags;});
            //set map1 input all along the x axis, check map2 reaction;
            seq.__step([&input_sampler, &map1,&map2, &pos_map1](){
              input_sampler();
              pos_map1+=0.01;
              if(pos_map1>1) pos_map1 = 0;
              map1.next(pos_map1);  //we'll plot cortical activity onlyZ
              map2.next(0); //on s'en fiche de l'entrée 2, on a fermé le thalamic.
            });
            seq.__update();
            seq.__step([&map2](){
              map2.save_act("map2.data", true);
            });
            seq.__();
          seq.__else();
            seq.__plot([&flags](){return flags;});
            seq.__step([&map1,&map2,&input_sampler](){
              input_sampler();
              map1.next(input_sampler[0]);
              map2.next(input_sampler[1]);
            });
            seq.__update(); //map1 update, map2 update
            seq.__learn(); //map1 learn, map2 learn
            seq.__();
          seq.__fi();
  /* */ /*seq.__pool();
}


//Séquence calculant les erreurs en fonction de BETA
template<typename INPUTSAMPLER>
void def_sequence_erreur(xsom::setup::Sequencer& seq,  State_1& map1,  State& map2,  INPUTSAMPLER& input_sampler,const std::string& flags, double& pos_map1,
                        std::vector<double>& pos_err_with_inputs, std::vector<double>& err_moy_with_inputs,
                        std::vector<double>& pos_err_without_inputs, std::vector<double>& err_moy_without_inputs,
                        std::vector<double>& x_without_in, std::vector<double>& y_without_in,
                        std::vector<double>& x_with_in, std::vector<double>& y_with_in,
                        std::vector<double>& inputx, std::vector<double>& inputy,
                        std::vector<double>& betas, int& step){
  //no menus
  //no interaction
  seq.__def("learning_loop");
    seq.__for(2000);
      seq.__step([&map1,&map2,&input_sampler, &pos_map1, &inputy,&inputx](){
              input_sampler();
              map1.next(input_sampler[0]);
              map2.next(input_sampler[1]);
              inputx.push_back(input_sampler[0]);
              inputy.push_back(input_sampler[1]);
            });
      seq.__update(); //map1 update, map2 update
      seq.__learn(); //map1 learn, map2 learn
    seq.__rof();
  seq.__fed();

  seq.__def("testing_loop");
    seq.__step([&inputx,&inputy](){
      inputx.clear();
      inputy.clear();
    });
    seq.__for(100);
    //next input
      seq.__step([&map1,&map2,&pos_map1,&input_sampler,&inputx,&inputy](){
              pos_map1+=2*3.14/100;
              if(pos_map1>2*3.14) pos_map1 = 0;
              map1.next(0.5 + 0.5*std::cos(pos_map1));
              map2.next(0.5 + 0.5*std::sin(pos_map1)); //positive
              inputx.push_back(0.5 + 0.5*std::cos(pos_map1));
              inputy.push_back(0.5 + 0.5*std::sin(pos_map1));
            });
      //map1 update, map2 update
      seq.__update();
      //calculate square error on input_space.
      seq.__step([&pos_err_with_inputs,&map1,&map2,&pos_map1](){
        double pos_map2 = 0.5+0.5*std::sin(pos_map1);
        double w1 = map1.tw(map1.bmu);
        double w2 = map2.tw(map2.bmu);
        double err = (std::abs(pos_map1) - std::abs(w1))*(std::abs(pos_map1) - std::abs(w1))
                    + (std::abs(pos_map2) - std::abs(w2))*(std::abs(pos_map2) - std::abs(w2)); //erreur quadratique.
        pos_err_with_inputs.push_back(err);
      });
      seq.__step([&x_with_in,&y_with_in,&map1,&map2,&step](){
        x_with_in.push_back(map1.tw(map1.bmu));
        y_with_in.push_back(map2.tw(map2.bmu));
      });
    seq.__rof();
    //calculate mean error and push it
    seq.__step([&pos_err_with_inputs, &err_moy_with_inputs](){
      double err_moy =0;
      for(auto& e : pos_err_with_inputs){
        err_moy += e;
      }
      err_moy = err_moy/pos_err_with_inputs.size();
      err_moy_with_inputs.push_back(err_moy);
    });
    //set Beta to 0 and realise test without map2 thalamic input.
    seq.__step([&map2,&pos_map1](){
      map2.close_thalamic();
      pos_map1 = 0;
    });
    //test on all positions
    seq.__step([&inputx,&inputy](){
      inputx.clear();
      inputy.clear();
    });
    seq.__for(100);
      seq.__step([&map1,&map2,&input_sampler,&pos_map1,&inputx,&inputy](){
              pos_map1+=2*3.14/100;
              if(pos_map1>2*3.14) pos_map1 = 0;
              map1.next(0.5 + 0.5*std::cos(pos_map1));
              //Map 2 est à 0.
              map2.next(0.5 + 0.5*std::sin(pos_map1));

              inputx.push_back(0.5 + 0.5*std::cos(pos_map1));
              inputy.push_back(0.5 + 0.5*std::sin(pos_map1));

            });
      //map1 update, map2 update
      seq.__update();
      //calculate errors
      seq.__step([&pos_err_without_inputs,&map1,&map2,&pos_map1](){
        double pos_map2 = 0.5+0.5*std::sin(pos_map1);
        double w1 = map1.tw(map1.bmu);
        double w2 = map2.tw(map2.bmu);
        double err = (std::abs(pos_map1) - std::abs(w1))*(std::abs(pos_map1) - std::abs(w1))
                    + (std::abs(pos_map2) - std::abs(w2))*(std::abs(pos_map2) - std::abs(w2)); //erreur quadratique.
        pos_err_without_inputs.push_back(err);
      });

      seq.__step([&x_without_in,&y_without_in,&map1,&map2,&step](){
        x_without_in.push_back(map1.tw(map1.bmu));
        y_without_in.push_back(map2.tw(map2.bmu));
      });

    seq.__rof();
    //plot step
    seq.__plot_png([&flags](){return flags;}, FIG_PREFIX);
    //calculate mean error and save it
      seq.__step([&pos_err_without_inputs, &err_moy_without_inputs](){
      double err_moy =0;
      for(auto& e : pos_err_without_inputs){
        err_moy += e;
      }
      err_moy = err_moy/pos_err_without_inputs.size();
      err_moy_without_inputs.push_back(err_moy);
    });
  seq.__fed();

  seq.__loop();
    seq.__for(20);
    seq.__step([&map2,&betas,&step](){
      map2.open_thalamic();

    });
    seq.__call("learning_loop");
    seq.__call("testing_loop");
    seq.__step([&step,&err_moy_with_inputs,&err_moy_without_inputs,&x_with_in,&x_without_in,&y_with_in,&y_without_in,&map1,&map2,&inputx,&inputy](){
    step++;
    err_moy_with_inputs.clear();
    err_moy_without_inputs.clear();
    x_with_in.clear();
    x_without_in.clear();
    y_with_in.clear();
    y_without_in.clear();
    inputx.clear();
    inputy.clear();
    std::random_device rd;
    std::mt19937 gen(rd());
    map1.reinit(gen);
    map2.reinit(gen);


    std::cout<<step<<std::endl;
    });
    seq.__rof();
    //quit
    seq.__step([](){
      throw xsom::instr::Exit();
    });
  seq.__pool();
}


template<typename INPUTSAMPLER>
void def_sequence_1D(xsom::setup::Sequencer& seq,  State_1& map1, INPUTSAMPLER& input_sampler, const std::string& flags){

  seq.interactive(true); // call this after having added menus.

  /* */ /*seq.__loop();
            seq.__update_and_learn();
            seq.__plot([&flags](){return flags;});
            seq.__step([&map1,&input_sampler](){
              input_sampler();
              map1.next(input_sampler[0]);
            });
            seq.__();
  /* *//* seq.__pool();
}*/
