#include "Display.hpp"
#include "State.hpp"
#include "../classes/InputSampler.hpp"
#include <string>
#include <ctime>

#define FILENAME "../files/inputs_2D_shape1.txt"
#define TEST_FILENAME "../files/test_inputs_2D_shape1.txt"
#define N_INPUTS 1000
#define N_TEST 400
#define VIEWER_PREFIX "2_SOM_1D"
#define PORT_NAME  10000
#define DAT_PREFIX "../experiences/2SOM_same/"
#define BATCH_SIZE 50


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
  State map2(gen);

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


int iterations = 1;
//init params in tab

Random_Circle_2D input_sampler(N_INPUTS, FILENAME, N_TEST, TEST_FILENAME, 0.5, 0.5, 0.5);
//Random_lissa input_sampler(N_INPUTS, FILENAME, 400, TEST_FILENAME);
  //initiate inputs;
input_sampler();
map1.next({input_sampler.get().first,input_sampler.get().second});
map2.next({input_sampler.get().first,input_sampler.get().second});

ccmpl::Main m(argc, argv, VIEWER_PREFIX);

auto display = ccmpl::layout(m.hostname, m.port, 16.0, 8.,{"##"},
                            ccmpl::RGB(1., 1., 1.));
display.set_ratios({1.,1.},{2.});
std::string flags = "" ;

//Generate display set up

plot_positions(std::ref(flags),std::ref(display),std::ref(map1), std::ref(map2), std::ref(results));

m.generate(display, true); // true means "use GUI".


//Architecture creation

auto archi = xsom::setup::network();

//MAP1

auto tl1 = xsom::layer<TWeight, Pos>(params._alpha(std::ref(iterations)),
            map1.x,
            std::bind(t_match, std::ref(params), _1, _2, _3),
            std::bind(h,         std::ref(params), std::ref(map1.bmu), _1),
            std::bind(&TWeights::get,   std::ref(map1.tw),  _1),
            std::bind(&Acts::learn,     std::ref(map1.ta),  _1),
            std::bind(&TWeights::learn, std::ref(map1.tw),  _1));

auto cl1 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iterations)),
              map2.bmu,
              std::bind(c_match, std::ref(params), _1, _2, _3),
              std::bind(h,         std::ref(params), std::ref(map1.bmu), _1),
              std::bind(&CWeights::get,   std::ref(map1.cw),  _1),
              std::bind(&Acts::learn,     std::ref(map1.ca),  _1),
              std::bind(&CWeights::learn, std::ref(map1.cw),  _1));

auto m1 = xsom::map(map1.bmu,
         std::bind(&Map::learn,               std::ref(map1.ma), _1),
         std::bind(merge,std::ref(map1.beta), std::ref(map1.ta),std::ref(map1.ca), _1),
         [&ma = map1.ma, &rd]() {return ma.random_bmu(rd);},
         std::bind(update_output_vit_prop, _1, _2)
       );

m1 += tl1;
m1+=cl1;
archi += m1;

//MAP2
auto tl2 = xsom::layer<TWeight, Pos>(params._alpha(std::ref(iterations)),
              map2.x,
              std::bind(t_match, std::ref(params), _1, _2, _3),
              std::bind(h,                std::ref(params),std::ref(map2.bmu), _1),
              std::bind(&TWeights::get,   std::ref(map2.tw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ta),  _1),
              std::bind(&TWeights::learn, std::ref(map2.tw),  _1));

  auto cl2 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iterations)),
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
           std::bind(update_output_vit_prop, _1, _2)
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

seq.__loop();
  seq.__for(1000);
    seq.__step([&input_sampler,&map1,&map2,&iterations](){
      input_sampler();
      map1.next({input_sampler.get().first, input_sampler.get().second});
      map2.next({input_sampler.get().first, input_sampler.get().second});
      map1.reinit_bmu(0.5);
      map2.reinit_bmu(0.5);
      iterations++;
    });

    seq.__for(70);
      seq.__update();
  seq.__rof();
  seq.__learn();
  seq.__();
  seq.__rof();
  seq.__step([&input_sampler](){
    input_sampler.test_mode = true;
  });
  seq.__for(200);
  seq.__step([&input_sampler,&map1,&map2,&results](){
    input_sampler();
    map1.next({input_sampler.get().first, input_sampler.get().second});
    map2.next({input_sampler.get().first, input_sampler.get().second});
    results.inputsx.push_back({input_sampler.get().first, input_sampler.get().second});
    map1.reinit_bmu(0.5);
    map2.reinit_bmu(0.5);
  });
  seq.__for(70);
    seq.__update();
seq.__rof();
  seq.__step([&results,&map1,&map2](){
    results.bmu_w1.push_back(map1.tw(map1.bmu));
    results.bmu_w2.push_back(map2.tw(map2.bmu));
  });
  seq.__();
  seq.__rof();
  seq.__plot_pdf([&flags](){return flags;},figure_prefix);
  seq.__step([](){
    throw xsom::instr::Exit();
});
seq.__pool();

seq.run();

return 0;

}
