#include "megasampler.h"
#include <iostream>

MEGASampler::MEGASampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy): Sampler(input,max_samples,max_time,max_epoch_samples,max_epoch_time,strategy),simpl_formula(c){
    	std::cout<<"starting MEGA"<<std::endl;
}




