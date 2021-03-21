#include "smtsampler.cpp"
#include "megasampler.h"

double duration(struct timespec * a, struct timespec * b) {
    return (b->tv_sec - a->tv_sec) + 1.0e-9 * (b->tv_nsec - a->tv_nsec);
}

double get_time_diff(struct timespec start_time){
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    return duration(&start_time, &end);
}

int main(int argc, char * argv[]) {
    int max_samples = 1000000;
    double max_time = 3600.0;
    int strategy = STRAT_SMTBIT;
    if (argc < 2) {
        std::cout << "Argument required: input file\n";
        return 0;
    }
    bool arg_samples = false;
    bool arg_time = false;
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-n") == 0)
            arg_samples = true;
        else if (strcmp(argv[i], "-t") == 0)
            arg_time = true;
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
    }
    SMTSampler s(argv[argc-1], max_samples, max_time, strategy);
//    MEGASampler s(argv[argc-1], max_samples, max_time, strategy);
    s.parse_formula();
    if (strategy == STRAT_SAT){
    	std::cout<<"Conversion to SAT is temporarily not supported"<<std::endl;
    	exit(0);
    }
    s.set_start_time();
    s.initialize_solvers();
    s.get_initial_model();
    double time = get_time_diff(s.get_start_time());
    std::cout<<"Time to find initial model: "<<time<<std::endl;
    s.calculate_coverage_under_model();
    s.compute_formula_statistics();
    s.print_formula_statistics();
    s.run();
    return 0;
}



