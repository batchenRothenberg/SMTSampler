#include "megasampler.h"
#include <iostream>

MEGASampler::MEGASampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy): Sampler(input,max_samples,max_time,max_epoch_samples,max_epoch_time,strategy),simpl_formula(c){
    	std::cout<<"starting MEGA"<<std::endl;
}

void MEGASampler::initialize_solvers(){
	std::cout<<"MEGA initializing solvers"<<std::endl;
	nnf_and_simplify_formula();
    opt.add(simpl_formula); //adds formula as hard constraint to optimization solver (no weight specified for it)
    solver.add(simpl_formula); //adds formula as constraint to normal solver
}

void MEGASampler::nnf_and_simplify_formula() {
    z3::tactic simplify(c, "simplify");
    z3::params p(c);
    p.set("arith_lhs",true);
    z3::tactic simp_with_params = with(simplify, p);
    z3::tactic nnf(c, "nnf");
    z3::tactic t_both = nnf & simp_with_params;
    z3::goal g(c);
    g.add(original_formula);

    set_timer_on("conversion");
    z3::apply_result res = t_both(g);
    simpl_formula = res.as_expr();
	std::cout<<res.as_expr()<<std::endl;
	accumulate_time("conversion");

}

