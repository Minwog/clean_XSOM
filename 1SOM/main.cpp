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
#define DAT_PREFIX "../experiences/SOM/"
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

ccmpl::Main m(argc, argv, VIEWER_PREFIX);

auto display = ccmpl::layout(m.hostname, m.port, 16.0, 8.,{"##"},
                            ccmpl::RGB(1., 1., 1.));
display.set_ratios({1.,1.},{2.});
std::string flags = "" ;

//Generate display set up

plot_positions(std::ref(flags),std::ref(display),std::ref(map1),std::ref(results));

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

auto m1 = xsom::map(map1.bmu,
         std::bind(&Map::learn,               std::ref(map1.ma), _1),
         std::bind(merge_simple,std::ref(map1.ta), _1),
         [&ma = map1.ma, &rd]() {return ma.random_bmu(rd);},
         std::bind(update_output, _1, _2)
       );

m1 += tl1;
archi += m1;



//Run sequence

auto seq   = xsom::setup::sequencer(archi, display);

bool showing_updates = false;
double pos_map1 = 0;
bool testing = false;
auto inputs = std::ofstream(input_filename);
auto bmus = std::ofstream(bmu_filename);

seq.__loop();
  seq.__for(5000);
    seq.__step([&input_sampler,&map1,&iterations](){
      input_sampler();
      map1.next({input_sampler.get().first, input_sampler.get().second});
      iterations++;
    });
    seq.__update_and_learn();
    seq.__();
  seq.__rof();
  seq.__step([&input_sampler](){
    input_sampler.test_mode = true;
  });
  seq.__for(200);
  seq.__step([&input_sampler,&map1,&results](){
    //input_sampler();
    std::random_device rd;
    std::mt19937 rng(rd());
    auto uniform = std::uniform_real_distribution<double>(0., 1);
    double res1 = uniform(rng); double res2 = uniform(rng);
    map1.next({res1,res2});
    results.inputsx.push_back({res1,res2});
  });
  seq.__update();
  seq.__step([&results,&map1](){
    results.bmu_w1.push_back(map1.tw(map1.bmu));
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
