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
#include <map>
#include <vector>
#include <algorithm> // for std::find


Z3_ast parse_bv(char const * n, Z3_sort s, Z3_context ctx);
std::string bv_string(Z3_ast ast, Z3_context ctx);

class Sampler{

    //Settings
    bool random_soft_bit = false; //TODO enable change from cmd line

    //Time management
	struct timespec start_time;
	struct timespec epoch_start_time;
	int max_samples;
	double max_time;
	int max_epoch_samples;
	double max_epoch_time;
    std::map<std::string, double> accumulated_times;

    //Formula statistics
    int num_arrays = 0, num_bv = 0, num_bools = 0, num_bits = 0, num_uf = 0, num_ints = 0, num_reals = 0;
    std::vector<z3::func_decl> variables;
    std::unordered_set<std::string> var_names = {"bv", "Int", "true", "false"}; //initialize with constant names so that constants are not mistaken for variables
    int max_depth = 0;
    std::unordered_set<Z3_ast> sup; //bat: nodes (=leaves?)

    //Other statistics
    int epochs = 0;
    int total_samples = 0; // how many samples we stumbled upon (repetitions are counted multiple times)
    int valid_samples = 0; // how many samples were valid (repetitions are counted multiple times)
    int unique_valid_samples = 0; //how many different valid samples were found (should always equal the size of the samples set and the number of lines in the results file)

    //Z3 objects
    z3::context c;
    z3::expr original_formula;
    z3::params params;
    z3::optimize opt;
    z3::solver solver;
    z3::model model;

    //Samples
    std::ofstream results_file;
    std::unordered_set<std::string> samples;


public:
    /*
     * Initializes limits and parameters.
     * Seeds random number generator.
     * Parses input file to get formula and initializes solvers with formula.
     * Computes formula statistics.
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
     * Generates and returns a model to begin a new epoch.
     * Accumulates time in seed_time.
     */
    z3::model start_epoch();
    /*
     * Sampling epoch: generates multiple valid samples from the given model.
     * Accumulates time in extension_time.
     * Whenever a sample is produced we check if it was produced before (i.e., belongs to the samples set).
     * If not, it is added to the samples set and output to the output file.
     */
    void do_epoch(const z3::model & model);
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
	void compute_and_print_formula_stats();
    void _compute_formula_stats_aux(z3::expr e, int depth = 0);
    void assert_soft(z3::expr const & e);
    void save_and_output_sample_if_unique(const std::string & sample);
    std::string model_to_string(const z3::model & model);
    /*
     * Assigns a random value to all variables and
     * adds equivalence constraints as soft constraints to opt.
     */
    void choose_random_assignment();
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
