#include "Simple_state.hpp"
#include "../classes/InputSampler.hpp"
#include "helpers.hpp"

template<typename INPUTSAMPLER>
void def_sequence_retro(xsom::setup::Sequencer& seq,  State& map1,  State& map2,
  bool& testing, INPUTSAMPLER& input_sampler,
  const std::string& flags, const std::string& date, std::string& file_prefix,
  Results& res, std::string& figure_prefix, int& iterations, std::string& file_res, Params& params, std::ofstream& difference_file, bool& stop
){
    seq.interactive(true); // call this after having added menus.

    seq.__def("test_loop");
      seq.__step([&res](){
        res.inputsx.clear();
        res.inputsy.clear();
        //input_file<<"test"<<"\n";
      });

      seq.__for(100);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx.push_back(input_sampler.get().first);
          res.inputsy.push_back(input_sampler.get().second);
          //input_sampler.save(input_file);
              });
              seq.__step([&map1,&map2](){
                Pos th_bmu1 = map1.compute_bmu_thal();
                Pos th_bmu2 = map2.compute_bmu_thal();
                map1.reinit_bmu(th_bmu1);
                map2.reinit_bmu(th_bmu2);
              });
        //map1 update, map2 update
        seq.__for(70);
          seq.__update();
          seq.__step([&map1, &map2,&difference_file](){
            difference_file<<map1.bmu<<";"<<map2.bmu <<";";
          });
        seq.__rof();
        /*seq.__step([&difference_file](){
        difference_file<<"\n";*/
      //});
      //seq.__plot_pdf([&flags](){return flags;},"../experiences/2SOM_retro/act_bothinp");
        seq.__step([&map1,&map2,&res](){
          //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
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
      seq.__for(100);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx_closed.push_back(input_sampler.get().first);
          res.inputsy_closed.push_back(input_sampler.get().second);
          //input_sampler.save(input_file);
              });
              //on met le bmu thal au milieu
              seq.__step([&map1,&map2](){
                Pos th_bmu1 = map1.compute_bmu_thal();
                //Pos th_bmu2 = map2.compute_bmu_thal();
                map1.reinit_bmu(th_bmu1);
                map2.reinit_bmu(0.5);
              });
        //map1 update, map2 update
        seq.__for(70);
          seq.__update();
        seq.__rof();
        //seq.__plot_pdf([&flags](){return flags;},"../experiences/2SOM_retro/act_map2");
        seq.__step([&map1,&map2,&res](){
          res.bmu_w1_closed.push_back(map1.tw(map1.bmu));
          res.bmu_w2_closed.push_back(map2.tw(map2.bmu));
             //res.bmu_1.push_back(map1.bmu);
          //res.bmu_2.push_back(map2.bmu);
          //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
        });

        seq.__();
      seq.__rof();
        seq.__step([&map1,&map2,&seq](){
        map1.close_thalamic();
        map2.open_thalamic();
        seq.msg_info("Test closing map1");
      });
      seq.__for(100);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx_closed2.push_back(input_sampler.get().first);
          res.inputsy_closed2.push_back(input_sampler.get().second);
          //input_sampler.save(input_file);
              });
              //on met le bmu thal au milieu
              seq.__step([&map1,&map2](){
                Pos th_bmu2= map2.compute_bmu_thal();
                //Pos th_bmu2 = map2.compute_bmu_thal();
                map2.reinit_bmu(th_bmu2);
                map1.reinit_bmu(0.5);
              });
        //map1 update, map2 update
        seq.__for(70);
          seq.__update();
        seq.__rof();
      //  seq.__plot_pdf([&flags](){return flags;},"../experiences/2SOM_retro/act_map1");
        seq.__step([&map1,&map2,&res](){
          res.bmu_w1_closed2.push_back(map1.tw(map1.bmu));
          res.bmu_w2_closed2.push_back(map2.tw(map2.bmu));
          //res.bmu_1.push_back(map1.bmu);
          //res.bmu_2.push_back(map2.bmu);
          //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
        });

        seq.__();
      seq.__rof();
    seq.__fed();

    seq.__def("learn_batch");
    seq.__step([&seq,&iterations,&params](){
      seq.msg_info(std::to_string(iterations));
      std::cerr<<params._alpha(iterations)<<std::endl;
    //  if(params.h_radius > H_RADIUS_MIN) params.h_radius = H_RADIUS_MAX - iterations * (H_RADIUS_MAX-H_RADIUS_MIN)/N_IT;
    });
    seq.__for(50);
      seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx_learn.push_back(input_sampler.get().first);
          res.inputsy_learn.push_back(input_sampler.get().second);
          //input_sampler.save(input_file);
              });
      seq.__update();
      seq.__plot([&flags](){return flags;});
     seq.__step([&map1,&map2](){
        Pos th_bmu1 = map1.compute_bmu_thal();
        Pos th_bmu2 = map2.compute_bmu_thal();
        map1.reinit_bmu(th_bmu1);
        map2.reinit_bmu(th_bmu2);
      });
      seq.__for(70);
        seq.__update();
        //seq.__plot([&flags](){return flags;});
        seq.__();
      seq.__rof();
      //seq.__plot([&flags](){return flags;});
      seq.__learn();
      seq.__step([&map1,&map2,&input_sampler,&res,&date,&file_prefix,&iterations](){
        res.bmu_w1_learning.push_back(map1.tw(map1.bmu));
        res.bmu_w2_learning.push_back(map2.tw(map2.bmu));
        iterations++;
        //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
      });
      seq.__();
    seq.__rof();
    seq.__fed();

    seq.__def("test_batch");
    seq.__step([&seq,&iterations](){
      seq.msg_info("test"+std::to_string(iterations));
    });
    seq.__for(20);
      seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
          res.inputsx.push_back(input_sampler.get().first);
          res.inputsy.push_back(input_sampler.get().second);
          //input_sampler.save(input_file);
              });
      seq.__update();
      seq.__step([&map1,&map2](){
        Pos th_bmu1 = map1.compute_bmu_thal();
        Pos th_bmu2 = map2.compute_bmu_thal();
        map1.reinit_bmu(th_bmu1);
        map2.reinit_bmu(th_bmu2);
      });
      seq.__for(70);
        seq.__update();
        seq.__();
      seq.__rof();
      seq.__step([&map1,&map2,&input_sampler,&res,&date,&file_prefix,&iterations](){
        //map1.save(file_prefix + "map1_w_"  + date +".data",true);
        //map2.save(file_prefix + "map2_w_"  + date +".data", true);
        res.bmu_w1_learning.push_back(map1.tw(map1.bmu));
        res.bmu_w2_learning.push_back(map2.tw(map2.bmu));
        iterations++;
        //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
      });
      seq.__();
    seq.__rof();
    seq.__fed();

    seq.__def("learn_loop");
    seq.__for((int) N_IT/BATCH_SIZE);
      seq.__call("learn_batch");
    /*  seq.__step([&res,&iterations](){
        compute_mean_error(std::ref(res), iterations);
      });*/
      seq.__();
    seq.__rof();
    seq.__fed();

  /* */ seq.__loop();
            seq.__call("learn_loop");
            seq.__step([&map1,&map2,&date,&file_prefix](){
              map1.save(file_prefix + "map1_w_"  + date +".data",false);
              map2.save(file_prefix + "map2_w_"  + date +".data", false);
            });
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
