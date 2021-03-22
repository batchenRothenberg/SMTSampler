/*
 * sampler.h
 *
 *  Created on: 21 Mar 2021
 *      Author: batchen
 */

#ifndef SAMPLER_H_
#define SAMPLER_H_

//#include "megasampler.h"
//#include "smtsampler.cpp"
#include <z3++.h>
#include <fstream> //for results_file
#include <unordered_set>

class Sampler{

    std::ofstream results_file;

	struct timespec start_time;
	double seed_time = 0.0;
	double extension_time = 0.0;
	int max_samples;
	double max_time;
	int max_epoch_samples;
	double max_epoch_time;

    z3::context c;
    z3::expr original_formula;
    z3::params params;
    z3::optimize opt;
    z3::solver solver;
    z3::model model;

    int total_samples = 0;
    std::unordered_set<std::string> samples;

public:
    /*
     * Initializes limits and parameters.
     * Parses input file to get formula.
     * Creates output file (stored in results_file).
     */
    Sampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy);
    /*
     * Checks if original_formula is satisfiable.
     * If so, stores a model of it in model.
     */
	void get_initial_model();
    /*
     * Checks if one of the global limits (time,samples) was reached.
     */
    bool is_limit_reached();
    /*
     * Checks if one of the epoch limits (time,samples) was reached.
     */
    bool is_epoch_limit_reached();
    /*
     * Generates and returns a model to begin a new epoch.
     */
    z3::model start_epoch();
    /*
     * Sampling epoch: generates multiple valid samples from the given model.
     * Whenever a sample is produced we check if it was produced before (i.e., belongs to the samples set).
     * If not, it is added to the samples set and output to the output file.
     */
    z3::model do_epoch(const z3::model & model);


private:
	double duration(struct timespec * a, struct timespec * b);
	double get_time_diff(struct timespec start_time);
	void parse_formula(std::string input);
	/*
	 * Tries to solve optimized formula (using opt).
	 * If too long, resorts to regular formula (using solver).
	 * Check result (sat/unsat/unknown) is returned.
	 * If sat - model is put in model variable.
	 */
	z3::check_result solve();
};



#endif /* SAMPLER_H_ */
