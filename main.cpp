#include "smtsampler.cpp"
//#include "megasampler.h"
#include "sampler.h"


int main(int argc, char * argv[]) {
    int max_samples = 1000000;
    double max_time = 3600.0;
    int max_epoch_samples = 10000;
    double max_epoch_time = 600.0;
    int strategy = STRAT_SMTBIT;
    if (argc < 2) {
        std::cout << "Argument required: input file\n";
        return 0;
    }
    bool arg_samples = false;
    bool arg_time = false;
    bool arg_epoch_samples = false;
    bool arg_epoch_time = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-n") == 0)
            arg_samples = true;
        else if (strcmp(argv[i], "-t") == 0)
            arg_time = true;
        else if (strcmp(argv[i], "-en") == 0)
            arg_epoch_samples = true;
        else if (strcmp(argv[i], "-et") == 0)
            arg_epoch_time = true;
        else if (strcmp(argv[i], "--smtbit") == 0)
            strategy = STRAT_SMTBIT;
        else if (strcmp(argv[i], "--smtbv") == 0)
            strategy = STRAT_SMTBV;
        else if (strcmp(argv[i], "--sat") == 0)
            strategy = STRAT_SAT;
        else if (arg_samples) {
            arg_samples = false;
            max_samples = atoi(argv[i]);
        } else if (arg_time) {
            arg_time = false;
            max_time = atof(argv[i]);
        }
        else if (arg_epoch_samples) {
            arg_epoch_samples = false;
            max_epoch_samples = atoi(argv[i]);
        } else if (arg_epoch_time) {
            arg_epoch_time = false;
            max_epoch_time = atof(argv[i]);
        }
    }

    if (strategy == STRAT_SAT){
    	std::cout<<"Conversion to SAT is temporarily not supported"<<std::endl;
    	exit(0);
    }

    Sampler s(argv[argc-1], max_samples, max_time, max_epoch_samples, max_epoch_time, strategy);
    s.check_if_satisfiable();


//    double time = get_time_diff(s.get_start_time());
//    std::cout<<"Time to find initial model: "<<time<<std::endl;
    //s.calculate_coverage_under_model();
//    s.compute_formula_statistics();
//    s.print_formula_statistics();
//    s.run();
    return 0;
}



