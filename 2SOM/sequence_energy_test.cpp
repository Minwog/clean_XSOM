#include "Simple_state.hpp"
#include "../classes/InputSampler.hpp"
#include "helpers.hpp"

template<typename INPUTSAMPLER>
void def_sequence_energy(xsom::setup::Sequencer& seq,  State& map1,  State& map2,
  bool& testing, INPUTSAMPLER& input_sampler,
  const std::string& flags, const std::string& date, std::string& file_prefix,
  Results& res, std::string& figure_prefix, int& iterations, std::string& file_res, Params& params, std::ofstream& difference_file, bool& stop
){
    seq.add_menu_item('s',"s","save figure, test loop and quit",[&seq,&stop](){
    stop = !stop;
  });
    seq.interactive(true); // call this after having added menus.

    seq.__def("test_loop");
      seq.__step([&res](){
        res.inputsx.clear();
        res.inputsy.clear();
        //input_file<<"test"<<"\n";
      });

      seq.__for(1000);
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
        seq.__step([&map1,&map2,&res](){
          //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
          res.bmu_w1.push_back(map1.tw(map1.bmu));
          res.bmu_w2.push_back(map2.tw(map2.bmu));
          res.bmu_1.push_back(map1.bmu);
          res.bmu_2.push_back(map2.bmu);
        });

        seq.__();
      seq.__rof();
    seq.__fed();

    seq.__def("learn_batch");
    seq.__step([&seq,&iterations,&params](){
      seq.msg_info(std::to_string(iterations));
      if(params.h_radius > H_RADIUS_MIN) params.h_radius = H_RADIUS_MAX - iterations * (H_RADIUS_MAX-H_RADIUS_MIN)/N_IT;
    });
    seq.__for(50);
      seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
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
        //seq.__plot([&flags](){return flags;});
        seq.__();
      seq.__rof();
      //seq.__plot([&flags](){return flags;});
      seq.__learn();
      seq.__step([&map1,&map2,&input_sampler,&res,&date,&file_prefix,&iterations](){
        //res.bmu_w1_learning.push_back(map1.tw(map1.bmu));
        //res.bmu_w2_learning.push_back(map2.tw(map2.bmu));
        iterations++;
        //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
      });
      seq.__();
    seq.__rof();
    seq.__fed();


    seq.__def("learn_loop_while");
    //seq.__if([&stop](){return !stop;});
      seq.__call("learn_batch");
      seq.__step([&map1,&map2,&date,&file_prefix](){
        map1.save(file_prefix + "map1_w_"  + date +".data",true);
        map2.save(file_prefix + "map2_w_"  + date +".data", true);
      });
      seq.__plot([&flags](){return flags;});
      seq.__step([&seq,&iterations](){seq.msg_info(std::to_string(iterations));});
    //seq.__else();
    //seq.__call("test_loop");
    /*seq.__step([&file_res,&res](){
      save_res_csv(std::ref(res), std::ref(file_res));
    });*/
    //seq.__plot_pdf([&flags](){return flags;}, figure_prefix);
    //seq.__step([](){throw xsom::instr::Exit();});
    //seq.__fi();
    seq.__fed();

  /* */ seq.__loop();
            seq.__call("learn_loop_while");
  /* */ seq.__pool();
}
