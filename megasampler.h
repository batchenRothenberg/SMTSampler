#ifndef MEGASAMPLER_H_
#define MEGASAMPLER_H_

#include <z3++.h>
#include <unordered_set>

class MEGASampler {
    z3::context c;
    const z3::model & model;
    const z3::expr & original_formula;
    z3::expr simpl_formula;
    int total_samples = 0;
    std::unordered_set<std::string> samples;

public:
    MEGASampler(const z3::expr & original_formula);
    /*
     * Finds additional valid models (samples) of the formula
     * (based on the given model, which is assumed valid).
     * Adds all valid samples to the samples set.
     * Returns the number of new samples found.
     */
    int sample(const z3::model & model);
    /*
     * Saves to the given file a list of all samples found during calls to sample.
     * File should already exist upon calling the function.
     */
    void save_valid_samples(std::string file);
    /*
     * Prints (to stdout) a list of all samples found during calls to sample.
     */
    void print_valid_samples();
};



#endif /* MEGASAMPLER_H_ */
