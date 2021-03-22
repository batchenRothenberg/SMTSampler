/*
 * sampler.h
 *
 *  Created on: 21 Mar 2021
 *      Author: batchen
 */

#ifndef SAMPLER_H_
#define SAMPLER_H_

#include <z3++.h>
#include <fstream> //for results_file
#include <unordered_set>

class Sampler{

    std::ofstream results_file;

	struct timespec start_time;
	struct timespec epoch_start_time;
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
     * Parses input file to get formula and initializes solvers with formula.
     * TODO Computes formula statistics.
     * Creates output file (stored in results_file).
     */
    Sampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy);
    /*
     * Checks if original_formula is satisfiable.
     * If not, or result is known, calls finish and exits.
     * If so, stores a model of it in model.
     */
    void check_if_satisfiable();
    /*
     * TODO Checks if the global time limit was reached.
     */
    bool is_time_limit_reached();
    /*
     * TODO Checks if one of the global limits (time,samples) was reached.
     */
    bool is_limit_reached();
    /*
     * TODO Checks if one of the epoch limits (time,samples) was reached.
     */
    bool is_epoch_limit_reached();
    /*
     * TODO Generates and returns a model to begin a new epoch.
     * Accumulates time in seed_time.
     */
    z3::model start_epoch();
    /*
     * TODO Sampling epoch: generates multiple valid samples from the given model.
     * Accumulates time in extension_time.
     * Whenever a sample is produced we check if it was produced before (i.e., belongs to the samples set).
     * If not, it is added to the samples set and output to the output file.
     */
    z3::model do_epoch(const z3::model & model);
    /*
     * Returns the time that has passed since the sampling process began (since this was created).
     */
    double get_elapsed_time();
    /*
     * Returns the time that has passed since the last epoch has began (since start_epoch was last called).
     */
    double get_epoch_elapsed_time();
    /*
     * Prints stats and closes results file.
     */
    void finish();

protected:
	double duration(struct timespec * a, struct timespec * b);
	double elapsed_time_from(struct timespec start);
	void parse_formula(std::string input);
	/*
	 * Tries to solve optimized formula (using opt).
	 * If too long, resorts to regular formula (using solver).
	 * Check result (sat/unsat/unknown) is returned.
	 * If sat - model is put in model variable.
	 */
	z3::check_result solve();
	/*
	 * Prints statistic information about the sampling procedure:
	 * number of samples and epochs and time spent on each phase.
	 */
	void print_stats();
};



#endif /* SAMPLER_H_ */
