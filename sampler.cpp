/*
 * sampler.cpp
 *
 *  Created on: 21 Mar 2021
 *      Author: batchen
 */
#include "sampler.h"

Sampler::Sampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy) : original_formula(c), max_samples(max_samples), max_time(max_time), max_epoch_samples(max_epoch_samples), max_epoch_time(max_epoch_time), params(c), opt(c), solver(c){
	z3::set_param("rewriter.expand_select_store", "true");
    clock_gettime(CLOCK_REALTIME, &start_time);
    params.set("timeout", 5000u);
    opt.set(params);
    solver.set(params);

	parse_formula(input);
    opt.add(original_formula); //adds formula as hard constraint to optimization solver (no weight specified for it)
    solver.add(original_formula); //adds formula as constraint to normal solver
}

double Sampler::_duration(struct timespec * a, struct timespec * b) {
    return (b->tv_sec - a->tv_sec) + 1.0e-9 * (b->tv_nsec - a->tv_nsec);
}

double Sampler::_get_time_diff(struct timespec start_time){
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    return _duration(&start_time, &end);
}

void Sampler::parse_formula(std::string input){
	std::cout<<"Parsing input file"<<std::endl;
	z3::expr formula = c.parse_file(input.c_str()); //bat: reads smt2 file
	Z3_ast ast = formula;
	if (ast == NULL) {
		std::cout << "Could not read input formula.\n";
		exit(1);
	}
	original_formula = formula;
}

