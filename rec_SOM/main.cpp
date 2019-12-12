#include "Display.hpp"
#include "Simple_state.hpp"
#include "../classes/InputSampler.hpp"
#include <string>
#include <ctime>

#define FILENAME "../files/inputs_2D_shape1.txt"
#define TEST_FILENAME "../files/test_inputs_2D_shape1.txt"
#define N_INPUTS 1000
#define N_TEST 100
#define VIEWER_PREFIX "rec_SOM"
#define DAT_PREFIX "../experiences/rec_SOM/"
#define PORT_NAME  10000

template<typename INPUTSAMPLER>
void def_sequence(xsom::setup::Sequencer& seq,  State& map1,INPUTSAMPLER& input_sampler,std::string& flags , Results& res, int& iterations, Params& params){


    seq.interactive(true); // call this after having added menus.


    seq.__def("test_loop");
    seq.__step([&res](){
      res.inputsx.clear();
    });
    seq.__for(50);
    //next input
      seq.__step([&seq, &map1,&input_sampler,&res](){
        input_sampler();
        map1.next(input_sampler.get());
        seq.msg_info(std::to_string(input_sampler.get()));
        res.inputsx.push_back(input_sampler.get());
            });
      //map1 update, map2 update
      seq.__update();
      seq.__plot([&flags](){return flags;});
      seq.__step([&map1,&res](){
      res.bmu_w1.push_back(map1.tw(map1.bmu));
      res.bmu_1.push_back(map1.bmu);
      });
      seq.__();
    seq.__rof();
    seq.__step([&map1,&seq](){
      map1.close_thalamic();
      seq.msg_info("close x");
    });
    seq.__for(50);
    //next input
      seq.__step([&map1,&input_sampler,&res](){
        input_sampler();
        map1.next(input_sampler.get());
        res.inputsx.push_back(input_sampler.get());
            });
      //map1 update, map2 update
      seq.__update();
      seq.__plot([&flags](){return flags;});
      seq.__step([&map1,&res](){
      res.bmu_w1.push_back(map1.tw(map1.bmu));
      res.bmu_1.push_back(map1.bmu);
      });
      seq.__();
    seq.__rof();
    seq.__fed();



    seq.__def("learn_batch");
    seq.__step([&seq,&iterations](){
      seq.msg_info(std::to_string(iterations));
    });
    seq.__for(50);
      seq.__step([&seq,&map1,&input_sampler,&res](){
          input_sampler();
          map1.next(input_sampler.get());
          seq.msg_info(std::to_string(input_sampler.get()));
          res.inputsx.push_back(input_sampler.get());
              });
      seq.__update();
      seq.__learn();
    //  seq.__plot([&flags](){return flags;});
      seq.__step([&iterations,&params](){
        iterations++;
      });
      seq.__();
    seq.__rof();
    seq.__fed();

    seq.__loop();
            seq.__for(20);
            seq.__call("learn_batch");
            seq.__();
            seq.__rof();
            seq.__step([&input_sampler,&seq](){input_sampler.test_mode=true; seq.msg_info("Test");});
            seq.__call("test_loop");
            /*seq.__step([&file_res,&res](){
              save_res_csv(std::ref(res), std::ref(file_res));
            });*/
            //seq.__plot_pdf([&flags](){return flags;}, figure_prefix );
  	  //seq.__step([&seq](){seq.msg_info("saving image");});
            seq.__step([](){
              throw xsom::instr::Exit();
  	    });
    /* */ seq.__pool();
}

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
  State   map1(gen);

  Results results;
  Params params;

  int iteration = 1;
//Vecteurs pour sauver les données

  int step = 0;

  Random_1D input_sampler(N_INPUTS, FILENAME, 100, TEST_FILENAME);
  //initiate inputs;
  input_sampler();
  map1.next(input_sampler.get());

ccmpl::Main m(argc, argv, VIEWER_PREFIX);

auto display = ccmpl::layout(m.hostname, m.port, 16.0, 8.0,{"##"},
                            ccmpl::RGB(1., 1., 1.));
display.set_ratios({1.,1.},{2.});

std::string flags = "" ;

plot_positions(std::ref(flags),std::ref(display),std::ref(map1),std::ref(results));

m.generate(display, true); // true means "use GUI".
//Architecture
auto archi = xsom::setup::network();

//MAP
auto tl = xsom::layer<TWeight, Pos>(params._alpha(std::ref(iteration)),
              map1.x,
              t_match,
              std::bind(h,   std::ref(params),                std::ref(map1.bmu), _1),
              std::bind(&TWeights::get,   std::ref(map1.tw),  _1),
              std::bind(&Acts::learn,     std::ref(map1.ta),  _1),
              std::bind(&TWeights::learn, std::ref(map1.tw),  _1));

  auto cl = xsom::layer<CWeight, Pos>(params._alpha(std::ref(iteration)),
              map1._x,
              c_match,
              std::bind(h,    std::ref(params),            std::ref(map1.bmu), _1),
              std::bind(&CWeights::get,   std::ref(map1.cw),  _1),
              std::bind(&Acts::learn,     std::ref(map1.ca),  _1),
              std::bind(&CWeights::learn, std::ref(map1.cw),  _1));

  auto m1 = xsom::map(map1.bmu,
           std::bind(&Map::learn,               std::ref(map1.ma), _1),
           std::bind(merge, std::ref(map1.beta),std::ref(map1.ta), std::ref(map1.ca), _1),
           [&ma = map1.ma, &rd]() {return ma.random_bmu(rd);},
           //std::bind(update_output, std::ref(map1.stabilized), _1, _2, std::ref(map1.bmu_tab), std::ref(map1.diff_tab))
           std::bind(update_output, _1, _2)
         );

  m1 += tl;
  m1 += cl;
  archi += m1;

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

/*std::string input_filename = prefix + "inputs_"+date+".data";
std::string bmu_filename = prefix + "bmus_" + date + ".data";
std::string figure_prefix = prefix + "img-"+date;
std::string file_res = prefix + "res_"+date+".data";*/

def_sequence<Random_1D>(std::ref(seq), std::ref(map1), std::ref(input_sampler), std::ref(flags) , std::ref(results),std::ref(iteration), std::ref(params));

seq.run();


return 0;

}
