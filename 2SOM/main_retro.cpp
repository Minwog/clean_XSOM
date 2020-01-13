#include "Display_retro.hpp"
#include "Simple_state.hpp"
#include "../classes/InputSampler.hpp"
#include <string>
#include <ctime>
#include "sequence_film.cpp"
#include "helpers.hpp"


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
std::string diff_filename = prefix + "bmus_update_test_"+date+".data";
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

Random_Circle_2D input_sampler(N_INPUTS, FILENAME, N_TEST, TEST_FILENAME, 0.5, 0.5, 0.5);
//Random_lissa input_sampler(N_INPUTS, FILENAME, 1000, TEST_FILENAME);
  //initiate inputs;
input_sampler();
map1.next(input_sampler.get().first);
map2.next(input_sampler.get().second);


ccmpl::Main m(argc, argv, VIEWER_PREFIX);

auto display = ccmpl::layout(m.hostname, m.port, 13.0, 13.0,{"##","##","##"},
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
            std::bind(h_th,         std::ref(params), std::ref(map1.bmu), _1),
            std::bind(&TWeights::get,   std::ref(map1.tw),  _1),
            std::bind(&Acts::learn,     std::ref(map1.ta),  _1),
            std::bind(&TWeights::learn, std::ref(map1.tw),  _1));

auto cl1 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iteration)),
              map2.bmu,
              std::bind(c_match, std::ref(params), _1, _2, _3),
              std::bind(h_c,         std::ref(params), std::ref(map1.bmu), _1),
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
              std::bind(h_th,                std::ref(params),std::ref(map2.bmu), _1),
              std::bind(&TWeights::get,   std::ref(map2.tw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ta),  _1),
              std::bind(&TWeights::learn, std::ref(map2.tw),  _1));

  auto cl2 = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iteration)),
              map1.bmu,
              std::bind(c_match, std::ref(params), _1, _2, _3),
              std::bind(h_c,                std::ref(params),  std::ref(map2.bmu), _1),
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
auto difference_file = std::ofstream(diff_filename);
auto bmus = std::ofstream(bmu_filename);
bool stop = false;

def_sequence_film<Random_Circle_2D>(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(testing), std::ref(input_sampler), std::ref(flags) , std::ref(date), std::ref(prefix) , std::ref(results),std::ref(figure_prefix), std::ref(iteration), std::ref(file_res),std::ref(params), std::ref(difference_file), std::ref(stop));
//def_sequence_opti<Random_Circle_2D,std::mt19937 >(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(input_sampler), std::ref(flags) , std::ref(date),std::ref(params),std::ref(results), std::ref(figure_prefix),std::ref(file_res),std::ref(gen),std::ref(iterations));

seq.run();

return 0;

}
