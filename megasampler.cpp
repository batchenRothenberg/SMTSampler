#include "megasampler.h"
#include <iostream>

MEGASampler::MEGASampler(const z3::model & model, const z3::expr & original_formula, const z3::context & c) : model(model), original_formula(original_formula), c(c) {
    	std::cout<<"starting MEGA"<<std::endl;
}




