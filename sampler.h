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

class Sampler{

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

//    SMTSampler smt_sampler;
//    MEGASampler mega_sampler;

public:
    Sampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy);
private:
	double _duration(struct timespec * a, struct timespec * b);
	double _get_time_diff(struct timespec start_time);
	void parse_formula(std::string input);
};



#endif /* SAMPLER_H_ */
