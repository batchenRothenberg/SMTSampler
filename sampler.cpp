/*
 * sampler.cpp
 *
 *  Created on: 21 Mar 2021
 *      Author: batchen
 */
#include "sampler.h"

Sampler::Sampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy) : original_formula(c), max_samples(max_samples), max_time(max_time), max_epoch_samples(max_epoch_samples), max_epoch_time(max_epoch_time), params(c), opt(c), solver(c),model(c){
	z3::set_param("rewriter.expand_select_store", "true");
    clock_gettime(CLOCK_REALTIME, &start_time);
    params.set("timeout", 5000u);
    opt.set(params);
    solver.set(params);

	parse_formula(input);
    opt.add(original_formula); //adds formula as hard constraint to optimization solver (no weight specified for it)
    solver.add(original_formula); //adds formula as constraint to normal solver

    results_file.open(input + ".samples");
}

double Sampler::duration(struct timespec * a, struct timespec * b) {
    return (b->tv_sec - a->tv_sec) + 1.0e-9 * (b->tv_nsec - a->tv_nsec);
}

double Sampler::get_elapsed_time(){
	return elapsed_time_from(start_time);
}

double Sampler::get_epoch_elapsed_time(){
	return elapsed_time_from(epoch_start_time);
}

double Sampler::elapsed_time_from(struct timespec start){
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    return duration(&start, &end);
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

void Sampler::check_if_satisfiable(){
	z3::check_result result = solve(); // will try to solve the formula and put model in model variable
	if (result == z3::unsat) {
		std::cout << "Formula is unsat\n";
		finish();
		exit(0);
	} else if (result == z3::unknown) {
		std::cout << "Solver returned unknown\n";
		finish();
		exit(0);
	} else {
		std::cout<<"Formula is satisfiable\n";
	}
}

z3::check_result Sampler::solve(){
	z3::check_result result = z3::unknown;
	try {
		result = opt.check(); //bat: first, solve a MAX-SMT instance
	} catch (z3::exception except) {
		std::cout << "Exception: " << except << "\n";
		exit(1);
	}
	if (result == z3::sat) {
		model = opt.get_model();
	} else if (result == z3::unknown) {
		std::cout << "MAX-SMT timed out"<< "\n";
		try {
			result = solver.check(); //bat: if too long, solve a regular SMT instance (without any soft constraints)
		} catch (z3::exception except) {
			std::cout << "Exception: " << except << "\n";
			exit(1);
		}
		std::cout << "SMT result: " << result << "\n";
		if (result == z3::sat) {
			model = solver.get_model();
		}
	}
	return result;
}

bool Sampler::is_time_limit_reached(){
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    double elapsed = duration(&start_time, &now);
    if (elapsed >= max_time) {
        std::cout << "Stopping: timeout\n";
        finish();
    }
}

void Sampler::finish() {
    print_stats();
    results_file.close();
    exit(0);
}

void Sampler::print_stats(){
	std::cout<<"printing stats"<<std::endl;
	//TODO print all stats
}

z3::model Sampler::start_epoch(){
	std::cout<<"starting epoch"<<std::endl;
	return model;
	//TODO start epoch
}

void Sampler::do_epoch(const z3::model & model){
	std::cout<<"doing epoch"<<std::endl;
	//TODO do epoch
}
