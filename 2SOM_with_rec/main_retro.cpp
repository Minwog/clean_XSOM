#include "Display_retro.hpp"
#include "Simple_state.hpp"
#include "../classes/InputSampler.hpp"
#include <string>
#include <ctime>

#define FILENAME "../files/inputs_2D_shape1.txt"
#define TEST_FILENAME "../files/test_inputs_2D_shape1.txt"
#define N_INPUTS 1000
#define N_TEST 400
#define VIEWER_PREFIX "2_SOM_1D"
#define PORT_NAME  10000
#define DAT_PREFIX "../experiences/2SOM_retro/"
#define BATCH_SIZE 50


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

//Definition of the sequence. Scenario 1 : learning begin at the same time for both thalamic and cortical weights.

template<typename INPUTSAMPLER>
void def_sequence_retro(xsom::setup::Sequencer& seq,  State& map1,  State& map2,
  bool& testing, INPUTSAMPLER& input_sampler,
  const std::string& flags, const std::string& date, std::ofstream& input_file, std::string& file_prefix, std::ofstream& bmu_file,
  Results& res, std::string& figure_prefix, int& iterations, std::string& file_res, Params& params
){
    seq.interactive(true); // call this after having added menus.

    seq.__def("test_loop");
      seq.__step([&res,&input_file](){
        res.inputsx.clear();
        res.inputsy.clear();
        //input_file<<"test"<<"\n";
      });

      seq.__for(200);
      //next input
        seq.__step([&map1,&map2,&input_sampler,&res,&input_file](){
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
          seq.__();
        seq.__rof();
        seq.__step([&map1,&map2,&bmu_file,&res](){
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
      seq.__for(200);
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
          seq.__();
        seq.__rof();
        seq.__step([&map1,&map2,&res](){
          res.bmu_w1_closed.push_back(map1.tw(map1.bmu));
          res.bmu_w2_closed.push_back(map2.tw(map2.bmu));
          res.bmu_1.push_back(map1.bmu);
          res.bmu_2.push_back(map2.bmu);
          //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
        });

        seq.__();
      seq.__rof();
    seq.__fed();

    seq.__def("learn_batch");
    seq.__step([&seq,&iterations,&params](){
      seq.msg_info(std::to_string(iterations));
      std::cerr<<params._alpha(iterations)<<std::endl;
      params.h_radius = H_RADIUS_MAX - iterations * (H_RADIUS_MAX-H_RADIUS_MIN)/N_IT;
    });
    seq.__for(50);
      seq.__step([&map1,&map2,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get().first);
          map2.next(input_sampler.get().second);
        //  res.inputsx.push_back(input_sampler.get().first);
        //  res.inputsy.push_back(input_sampler.get().second);
          //input_sampler.save(input_file);
              });
      seq.__update();
      //seq.__plot([&flags](){return flags;});
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
      seq.__learn();
      seq.__step([&map1,&map2,&input_sampler,&res,&date,&file_prefix,&iterations](){
      //  res.bmu_w1_learning.push_back(map1.tw(map1.bmu));
      //  res.bmu_w2_learning.push_back(map2.tw(map2.bmu));
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
    seq.__for(50);
      seq.__step([&map1,&map2,&input_sampler,&res,&input_file](){
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
      seq.__step([&map1,&map2,&input_sampler,&res,&input_file,&date,&file_prefix,&bmu_file,&iterations](){
        //map1.save(file_prefix + "map1_w_"  + date +".data",true);
        //map2.save(file_prefix + "map2_w_"  + date +".data", true);
        //res.bmu_w1_learning.push_back(map1.tw(map1.bmu));
        //res.bmu_w2_learning.push_back(map2.tw(map2.bmu));
        iterations++;
        //bmu_file<<map1.bmu<<";"<<map2.bmu<<"\n";
      });
      seq.__();
    seq.__rof();
    seq.__fed();

    seq.__def("learn_loop");
    seq.__for(20);
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



int main(int argc, char* argv[]){

  if(argc == 1) {
    std::cout << std::endl
	      << std::endl
	      << "Usage :"
<< std::endl
	      << "  " << argv[0] << " display" << std::endl
	      << "  python3 ./" << VIEWER_PREFIX << ".py " << PORT_NAME << std::endl
	      << "  " << argv[0] << " localhost " << PORT_NAME<< std::endl
	      << std::endl
	      << std::endl
	      << std::endl;
    ::exit(0);
  }


  std::random_device rd;
  std::mt19937 gen(rd());

//Maps generation
  State map1(gen);
  State_1 map2(gen);

//File names definition

time_t my_time = time(0);
tm *ltm = localtime(&my_time);
std::string date= "";
date = date + std::to_string(1900 + ltm->tm_year) +"-" +std::to_string(1 + ltm->tm_mon) +"-"+std::to_string(ltm->tm_mday)+"_"+std::to_string(ltm->tm_hour)+"-"+std::to_string(ltm->tm_min);

std::string prefix =  DAT_PREFIX ;
std::string input_filename = prefix + "inputs_retro_"+date+".data";
std::string bmu_filename = prefix + "bmus_retro" + date + ".data";
std::string figure_prefix = prefix + "img-"+date;
std::string file_res = prefix + "res_"+date+".data";
int step = 0;

//Variables initialization


Results results;
//Params params = {alpha_tab[0], t_match_sigma_tab[0]*t_match_sigma_tab[0], c_match_sigma_tab[0]*c_match_sigma_tab[0], h_radius_tab[0]} ;
Params params;


int iteration = 1;
//init params in tab

//Random_Circle_2D input_sampler(N_INPUTS, FILENAME, N_TEST, TEST_FILENAME, 0.5, 0.5, 0.5);
Random_lissa input_sampler(N_INPUTS, FILENAME, 400, TEST_FILENAME);
  //initiate inputs;
input_sampler();
map1.next(input_sampler.get().first);
map2.next(input_sampler.get().second);


ccmpl::Main m(argc, argv, VIEWER_PREFIX);

auto display = ccmpl::layout(m.hostname, m.port, 16.0, 16.0,{"##","##","##"},
                            ccmpl::RGB(1., 1., 1.));
display.set_ratios({1.,1.},{1.,2.,2.});



std::string flags = "" ;

//Generate display set up

plot_positions(std::ref(flags),std::ref(display),std::ref(map1),std::ref(map2),std::ref(results));

m.generate(display, true); // true means "use GUI".


//Architecture creation

auto archi = xsom::setup::network();

//MAP1

auto tl1 = xsom::layer<TWeight, Pos>(params._alpha(std::ref(iteration)),
            map1.x,
            std::bind(t_match, std::ref(params), _1, _2, _3),
            std::bind(h,         std::ref(params), std::ref(map1.bmu), _1),
            std::bind(&TWeights::get,   std::ref(map1.tw),  _1),
            std::bind(&Acts::learn,     std::ref(map1.ta),  _1),
            std::bind(&TWeights::learn, std::ref(map1.tw),  _1));

auto cl1 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iteration)),
              map2.bmu,
              std::bind(c_match, std::ref(params), _1, _2, _3),
              std::bind(h,         std::ref(params), std::ref(map1.bmu), _1),
              std::bind(&CWeights::get,   std::ref(map1.cw),  _1),
              std::bind(&Acts::learn,     std::ref(map1.ca),  _1),
              std::bind(&CWeights::learn, std::ref(map1.cw),  _1));

auto m1 = xsom::map(map1.bmu,
         std::bind(&Map::learn,               std::ref(map1.ma), _1),
         std::bind(merge,std::ref(map1.beta),std::ref(map1.ta),std::ref(map1.ca), _1),
         [&ma = map1.ma, &rd]() {return ma.random_bmu(rd);},
         std::bind(update_output_vit_prop, _1, _2, std::ref(map1.bmus), std::ref(map1.diff))
       );

m1 += tl1;
m1+=cl1;
archi += m1;

//MAP2
auto tl2 = xsom::layer<TWeight, Pos>(params._alpha(std::ref(iteration)),
              map2.x,
              std::bind(t_match, std::ref(params), _1, _2, _3),
              std::bind(h,                std::ref(params),std::ref(map2.bmu), _1),
              std::bind(&TWeights::get,   std::ref(map2.tw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ta),  _1),
              std::bind(&TWeights::learn, std::ref(map2.tw),  _1));

auto cl2 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iteration)),
              map1.bmu,
              std::bind(c_match, std::ref(params), _1, _2, _3),
              std::bind(h,                std::ref(params),  std::ref(map2.bmu), _1),
              std::bind(&CWeights::get,   std::ref(map2.cw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ca),  _1),
              std::bind(&CWeights::learn, std::ref(map2.cw),  _1));

 auto dl2 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iteration)),
              map1.bmu,
              std::bind(c_match, std::ref(params), _1, _2, _3),
              std::bind(h,                std::ref(params),  std::ref(map2.bmu), _1),
              std::bind(&CWeights::get,   std::ref(map2.cw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ca),  _1),
              std::bind(&CWeights::learn, std::ref(map2.cw),  _1));

  auto m2 = xsom::map(map2.bmu,
           std::bind(&Map::learn,               std::ref(map2.ma), _1),
           std::bind(merge, std::ref(map2.beta),std::ref(map2.ta), std::ref(map2.ca), _1),
           [&ma = map2.ma, &rd]() {return ma.random_bmu(rd);},
           //std::bind(update_output, std::ref(map2.stabilized), _1, _2, std::ref(map2.bmu_tab), std::ref(map2.diff_tab))
           std::bind(update_output_vit_prop, _1, _2, std::ref(map2.bmus),std::ref(map2.diff))
         );

  m2 += tl2;
  m2 += cl2;
  archi += m2;

//Run sequence

auto seq   = xsom::setup::sequencer(archi, display);

bool showing_updates = false;
double pos_map1 = 0;
bool testing = false;
auto inputs = std::ofstream(input_filename);
auto bmus = std::ofstream(bmu_filename);

def_sequence_retro<Random_lissa>(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(testing), std::ref(input_sampler), std::ref(flags) , std::ref(date),std::ref(inputs), std::ref(prefix) , std::ref(bmus), std::ref(results),std::ref(figure_prefix), std::ref(iteration), std::ref(file_res),std::ref(params));
//def_sequence_opti<Random_Circle_2D,std::mt19937 >(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(input_sampler), std::ref(flags) , std::ref(date),std::ref(params),std::ref(results), std::ref(figure_prefix),std::ref(file_res),std::ref(gen),std::ref(iterations));

seq.run();
inputs.close();

return 0;

}
