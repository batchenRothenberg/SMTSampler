#ifndef MEGASAMPLER_H_
#define MEGASAMPLER_H_

#include "sampler.h"

class MEGASampler : public Sampler {

    z3::expr simpl_formula;

public:
    MEGASampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy);
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
