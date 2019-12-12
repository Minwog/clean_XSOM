#include "Sequences.hpp"
#include "Display.hpp"
#include "Simple_state.hpp"
#include "../classes/InputSampler.hpp"
#include <string>
#include <ctime>

#define FILENAME "../files/inputs_2D_shape1.txt"
#define TEST_FILENAME "../files/test_inputs_2D_shape1.txt"
#define N_INPUTS 1000
#define N_TEST 100
#define VIEWER_PREFIX "2_SOM_1D"
#define DAT_PREFIX "../experiences/2SOM_simple/"
#define PORT_NAME  10000


// Display functionalities
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

  //Maps
  State_1 map1(gen);
  State   map2(gen);
  //map3 : copier la thalamic de la map2
  State_1 map3(gen,std::cref(map2));

  Results results;
  Params params;

  int iteration = 1;
//Vecteurs pour sauver les données

  std::vector<double> pos_err_with_inputs, err_moy_with_inputs, pos_err_without_inputs, err_moy_without_inputs;
  std::vector<double> betas = {0.3, 0.4, 0.5, 0.6, 0.7, 0.75, 0.8,0.85, 0.9, 0.95};
  std::vector<double> x_without_in, y_without_in, x_with_in, y_with_in, inputsx, inputsy;

  int step = 0;

  //Random_Circle_2D input_sampler(N_INPUTS, FILENAME, 100, TEST_FILENAME,0.5,0.5,0.5);
  Random_lissa input_sampler(N_INPUTS, FILENAME, 100, TEST_FILENAME);
  //initiate inputs;
  input_sampler();
  map1.next(input_sampler.get().first);
  map2.next(input_sampler.get().second);


ccmpl::Main m(argc, argv, VIEWER_PREFIX);

auto display = ccmpl::layout(m.hostname, m.port, 16.0, 16.0,{"##","##","#."},
                            ccmpl::RGB(1., 1., 1.));
display.set_ratios({1.,1.},{1.,2.,2.});

/*auto display = ccmpl::layout(m.hostname, m.port, 16.0, 16.0,{"##",">."},
                            ccmpl::RGB(1., 1., 1.));
display.set_ratios({1.,1.},{1.,2.});*/

std::string flags = "" ;


//display_bmus(std::ref(map1), std::ref(map2), std::ref(map3), std::ref(flags2),std::ref(display2));
//display_activity(std::ref(map1), std::ref(map2), std::ref(flags),std::ref(display));
//display_weights_in_input_space(std::ref(map1), std::ref(map2), std::ref(flags),std::ref(display));
//plot_error(std::ref(flags),std::ref(display),std::ref(err_moy_with_inputs), std::ref(err_moy_without_inputs), std::ref(betas));
plot_positions(std::ref(flags),std::ref(display),std::ref(map1),std::ref(map2),std::ref(results));

m.generate(display, true); // true means "use GUI".
//Architecture
auto archi = xsom::setup::network();
//MAP1

auto tl1 = xsom::layer<TWeight, Pos>(params._alpha(iteration),
            map1.x,
            t_match,
            std::bind(h,    std::ref(params),            std::ref(map1.bmu), _1),
            std::bind(&TWeights::get,   std::ref(map1.tw),  _1),
            std::bind(&Acts::learn,     std::ref(map1.ta),  _1),
            std::bind(&TWeights::learn, std::ref(map1.tw),  _1));

auto m1 = xsom::map(map1.bmu,
         std::bind(&Map::learn,               std::ref(map1.ma), _1),
         std::bind(merge_simple,std::ref(map1.ta), _1),
         [&ma = map1.ma, &rd]() {return ma.random_bmu(rd);},
         std::bind(update_output, _1, _2, std::ref(map1.bmus))
       );

m1 += tl1;
archi += m1;

//MAP2
auto tl2 = xsom::layer<TWeight, Pos>(params._alpha(iteration),
              map2.x,
              t_match,
              std::bind(h,   std::ref(params),                std::ref(map2.bmu), _1),
              std::bind(&TWeights::get,   std::ref(map2.tw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ta),  _1),
              std::bind(&TWeights::learn, std::ref(map2.tw),  _1));

  auto cl2 = xsom::layer<CWeight, Pos>(params._alpha(iteration),
              map1.bmu,
              c_match,
              std::bind(h,    std::ref(params),            std::ref(map2.bmu), _1),
              std::bind(&CWeights::get,   std::ref(map2.cw),  _1),
              std::bind(&Acts::learn,     std::ref(map2.ca),  _1),
              std::bind(&CWeights::learn, std::ref(map2.cw),  _1));

  auto m2 = xsom::map(map2.bmu,
           std::bind(&Map::learn,               std::ref(map2.ma), _1),
           std::bind(merge, std::ref(map2.beta),std::ref(map2.ta), std::ref(map2.ca), _1),
           [&ma = map2.ma, &rd]() {return ma.random_bmu(rd);},
           //std::bind(update_output, std::ref(map2.stabilized), _1, _2, std::ref(map2.bmu_tab), std::ref(map2.diff_tab))
           std::bind(update_output, _1, _2, std::ref(map2.bmus))
         );

  m2 += tl2;
  m2 += cl2;
  archi += m2;


/*
  auto tl3 = xsom::layer<TWeight, Pos>(params.__alpha(iteration),
            map3.x,
            t_match,
            std::bind(h,                std::ref(map3.bmu), _1),
            std::bind(&TWeights::get,   std::ref(map3.tw),  _1),
            std::bind(&Acts::learn,     std::ref(map3.ta),  _1),
            std::bind(&TWeights::learn, std::ref(map3.tw),  _1));

auto m3 = xsom::map(map3.bmu,
         std::bind(&Map::learn,               std::ref(map3.ma), _1),
         std::bind(merge_simple,std::ref(map3.ta), _1),
         [&ma = map3.ma, &rd]() {return ma.random_bmu(rd);},
         std::bind(update_output, _1, _2, std::ref(map3.bmus))
       );

m3+=tl3;

archi+=m3;*/

//Let's build a sequencer for our steps
//Scenario 2 : cortical entries are shut down, we initialize maps with thalamic entries. Then we run learning with corical.
auto seq       = xsom::setup::sequencer(archi, display);

bool showing_updates = false;
double pos_map1 = 0;
bool testing = false;

time_t my_time = time(0);
tm *ltm = localtime(&my_time);
std::string date= "";
date = date + std::to_string(ltm->tm_year) +"-" +std::to_string(ltm->tm_mon) +"-"+std::to_string(ltm->tm_mday)+"_"+std::to_string(ltm->tm_hour)+"-"+std::to_string(ltm->tm_min);

std::string prefix =  DAT_PREFIX ;

std::string input_filename = prefix + "inputs_"+date+".data";
std::string bmu_filename = prefix + "bmus_" + date + ".data";
std::string figure_prefix = prefix + "img-"+date;
std::string file_res = prefix + "res_"+date+".data";




auto inputs = std::ofstream(input_filename);
auto bmus = std::ofstream(bmu_filename);
def_sequence<Random_lissa>(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(testing), std::ref(input_sampler), std::ref(flags) , std::ref(date),std::ref(inputs), std::ref(prefix) , std::ref(bmus), std::ref(results),std::ref(figure_prefix), std::ref(iteration), std::ref(file_res) ,std::ref(params));
//def_sequence_map2<InputSampler::Random_circle_2D>(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(testing), std::ref(input_sampler), std::ref(flags), std::ref(pos_map1) );

/*def_sequence_erreur<Random_Circle_2D>(std::ref(seq), std::ref(map1), std::ref(map2), std::ref(input_sampler), std::ref(flags), std::ref(pos_map1),
                    std::ref(pos_err_with_inputs), std::ref(err_moy_with_inputs),
                    std::ref(pos_err_without_inputs), std::ref(err_moy_without_inputs),
                    std::ref(x_without_in), std::ref(y_without_in), std::ref(x_with_in), std::ref(y_with_in),
                    std::ref(inputsx), std::ref(inputsy),
                    std::ref(betas), std::ref(step));*/
seq.run();


return 0;

}
