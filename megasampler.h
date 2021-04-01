#ifndef MEGASAMPLER_H_
#define MEGASAMPLER_H_

#include "sampler.h"

class MEGASampler : public Sampler {

    z3::expr simpl_formula;

public:
    MEGASampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy);
    void initialize_solvers();
protected:
    void nnf_and_simplify_formula();
};



#endif /* MEGASAMPLER_H_ */
