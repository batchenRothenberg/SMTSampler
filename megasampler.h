#ifndef MEGASAMPLER_H_
#define MEGASAMPLER_H_

#include <z3++.h>

class MEGASampler {
    const z3::model & model;
    const z3::expr & original_formula;
    const z3::context & c;
    int samples = 0;
    int valid_samples = 0;

public:
    MEGASampler(const z3::model & model, const z3::expr & original_formula, const z3::context & c);
};



#endif /* MEGASAMPLER_H_ */
