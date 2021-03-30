/*
 * sampler.cpp
 *
 *  Created on: 21 Mar 2021
 *      Author: batchen
 */
#include "sampler.h"

Sampler::Sampler(std::string input, int max_samples, double max_time, int max_epoch_samples, double max_epoch_time, int strategy) : original_formula(c), max_samples(max_samples), max_time(max_time), max_epoch_samples(max_epoch_samples), max_epoch_time(max_epoch_time), params(c), opt(c), solver(c),model(c){
	z3::set_param("rewriter.expand_select_store", "true");
    clock_gettime(CLOCK_REALTIME, &start_time);

    srand(start_time.tv_sec);

    params.set("timeout", 50000u);
    opt.set(params);
    solver.set(params);

	parse_formula(input);
    opt.add(original_formula); //adds formula as hard constraint to optimization solver (no weight specified for it)
    solver.add(original_formula); //adds formula as constraint to normal solver

    compute_and_print_formula_stats();

    results_file.open(input + ".samples");
}

double Sampler::duration(struct timespec * a, struct timespec * b) {
    return (b->tv_sec - a->tv_sec) + 1.0e-9 * (b->tv_nsec - a->tv_nsec);
}

double Sampler::get_elapsed_time(){
	return elapsed_time_from(start_time);
}

double Sampler::get_epoch_elapsed_time(){
	return elapsed_time_from(epoch_start_time);
}

double Sampler::elapsed_time_from(struct timespec start){
    struct timespec end;
    clock_gettime(CLOCK_REALTIME, &end);
    return duration(&start, &end);
}

void Sampler::parse_formula(std::string input){
	std::cout<<"Parsing input file: "<<input<<std::endl;
	z3::expr formula = c.parse_file(input.c_str()); //bat: reads smt2 file
	Z3_ast ast = formula;
	if (ast == NULL) {
		std::cout << "Could not read input formula.\n";
		exit(1);
	}
	original_formula = formula;
}

void Sampler::check_if_satisfiable(){
	z3::check_result result = solve(); // will try to solve the formula and put model in model variable
	if (result == z3::unsat) {
		std::cout << "Formula is unsat\n";
		finish();
		exit(0);
	} else if (result == z3::unknown) {
		std::cout << "Solver returned unknown\n";
		finish();
		exit(0);
	} else {
		std::cout<<"Formula is satisfiable\n";
	}
}

z3::check_result Sampler::solve(){
//	std::cout<<"Opt assertions:"<<std::endl;
//	std::cout<<opt.assertions()<<std::endl;
//	std::cout<<"Opt objectives:"<<std::endl;
//	std::cout<<opt.objectives()<<std::endl;
	z3::check_result result = z3::unknown;
	try {
		result = opt.check(); //bat: first, solve a MAX-SMT instance
	} catch (z3::exception except) {
		std::cout << "Exception: " << except << "\n";
		//TODO exception "canceled" can be thrown when Timeout is reached
		exit(1);
	}
	if (result == z3::sat) {
		model = opt.get_model();
	} else if (result == z3::unknown) {
		std::cout << "MAX-SMT timed out"<< "\n";
		try {
			result = solver.check(); //bat: if too long, solve a regular SMT instance (without any soft constraints)
		} catch (z3::exception except) {
			std::cout << "Exception: " << except << "\n";
			exit(1);
		}
		std::cout << "SMT result: " << result << "\n";
		if (result == z3::sat) {
			model = solver.get_model();
		}
	}
	return result;
}

bool Sampler::is_time_limit_reached(){
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    double elapsed = duration(&start_time, &now);
    if (elapsed >= max_time) {
        std::cout << "Stopping: timeout\n";
        finish();
    }
}

void Sampler::finish() {
    print_stats();
    results_file.close();
    exit(0);
}

void Sampler::print_stats(){
	std::cout<<"---------SOLVING STATISTICS--------"<<std::endl;
	for(auto it = accumulated_times.cbegin(); it != accumulated_times.cend(); ++it)
	{
	    std::cout << it->first << " time: " << it->second << std::endl;
	}
	std::cout<<"Epochs: "<<epochs<<std::endl;
	std::cout<<"Assignments considered (with repetitions): "<<total_samples<<std::endl;
	std::cout<<"Models (with repetitions): "<<valid_samples<<std::endl;
	std::cout<<"Unique models (# samples in file): "<<unique_valid_samples<<std::endl;
	std::cout<<"-----------------------------------"<<std::endl;
}

z3::model Sampler::start_epoch(){
	std::cout<<"Starting an epoch"<<std::endl;

    opt.push(); // because formula is constant, but other hard/soft constraints change between epochs
    choose_random_assignment();
    z3::check_result result = solve(); //bat: find closest solution to random assignment (or some solution)
    assert(result != z3::unsat);
    opt.pop();

    epochs++;
    total_samples++;
    valid_samples++;

//    save_and_output_sample_if_unique(Z3_model_to_string(c,model));
    save_and_output_sample_if_unique(model_to_string(model));

	return model;
}

void Sampler::choose_random_assignment(){
    for (z3::func_decl & v : variables) { //bat: Choose a random assignment: for variable-> if bv or bool, randomly choose a value to it.
		if (v.arity() > 0 || v.range().is_array())
			continue;
		switch (v.range().sort_kind()) {
			case Z3_BV_SORT: // random assignment to bv
			{
				if (random_soft_bit) {
					for (int i = 0; i < v.range().bv_size(); ++i) {
						if (rand() % 2)
							assert_soft(v().extract(i, i) == c.bv_val(0, 1));
						else
							assert_soft(v().extract(i, i) != c.bv_val(0, 1));
					}
				} else {
					std::string n;
					char num[10];
					int i = v.range().bv_size();
					if (i % 4) {
						snprintf(num, 10, "%x", rand() & ((1<<(i%4)) - 1));
						n += num;
						i -= (i % 4);
					}
					while (i) {
						snprintf(num, 10, "%x", rand() & 15);
						n += num;
						i -= 4;
					}
					Z3_ast ast = parse_bv(n.c_str(), v.range(), c);
					z3::expr exp(c, ast);
					assert_soft(v() == exp);
				}
				break; // from switch, bv case
			}
			case Z3_BOOL_SORT: // random assignment to bool var
				if (rand() % 2)
					assert_soft(v());
				else
					assert_soft(!v());
				break; // from switch, bool case
			case Z3_INT_SORT: // random assignment to bool var
			{
				int random = rand();
				if (rand() % 2)
					assert_soft(v() == c.int_val(random));
				else
					assert_soft(v() == c.int_val(-random));
			}
				break; // from switch, int case
			default:
				//TODO add real
				std::cout << "Invalid sort\n";
				exit(1);
		}
    } //end for: random assignment chosen
}

void Sampler::do_epoch(const z3::model & model){
	std::cout<<"Epoch: keeping only original model"<<std::endl;
}

void Sampler::compute_and_print_formula_stats(){
	// TODO save formula theory
	_compute_formula_stats_aux(original_formula);
//	std::cout << "Nodes " << sup.size() << '\n';
//	std::cout << "Internal nodes " << sub.size() << '\n';
	std::cout << "-------------FORMULA STATISTICS-------------" << '\n';
	std::cout << "Arrays " << num_arrays << '\n';
	std::cout << "Bit-vectors " << num_bv << '\n';
	std::cout << "Bools " << num_bools << '\n';
	std::cout << "Bits " << num_bits << '\n';
	std::cout << "Uninterpreted functions " << num_uf << '\n';
	std::cout << "Ints " << num_ints << '\n';
	std::cout << "Reals " << num_reals << '\n';
	std::cout << "Formula tree depth " << max_depth << '\n';
	std::cout << "--------------------------------------------" << '\n';

}

void Sampler::_compute_formula_stats_aux(z3::expr e, int depth){
    if (sup.find(e) != sup.end())
        return;
    assert(e.is_app());
    z3::func_decl fd = e.decl();
    if (e.is_const()) {
        std::string name = fd.name().str();
        if (var_names.find(name) == var_names.end()) {
            var_names.insert(name);
            variables.push_back(fd);
            if (fd.range().is_array()) {
               ++num_arrays;
            } else if (fd.is_const()) {
                switch (fd.range().sort_kind()) {
                case Z3_BV_SORT:
                    ++num_bv;
                    num_bits += fd.range().bv_size();
                    break;
                case Z3_BOOL_SORT:
                    ++num_bools;
                    ++num_bits;
                    break;
                case Z3_INT_SORT:
                    ++num_ints;
                    break;
                case Z3_REAL_SORT:
                    ++num_reals;
                    break;
                default:
                    std::cout << "Invalid sort\n";
                    exit(1);
                }
            }
        }
    } else if (fd.decl_kind() == Z3_OP_UNINTERPRETED) {
        std::string name = fd.name().str();
        if (var_names.find(name) == var_names.end()) {
            var_names.insert(name);
            // std::cout << "declaration: " << fd << '\n';
            variables.push_back(fd);
            ++num_uf;
        }
    }
//    if (e.is_bool() || e.is_bv()) {
//        sub.insert(e);
//    }
    sup.insert(e);
    if (depth > max_depth){
        max_depth = depth;
    }
    for (int i = 0; i < e.num_args(); ++i){
    	_compute_formula_stats_aux(e.arg(i), depth + 1);
    }
}

void Sampler::assert_soft(z3::expr const & e) {
    opt.add(e, 1);
}

void Sampler::save_and_output_sample_if_unique(const std::string & sample){
    auto res = samples.insert(sample);
    if (res.second) {
    	unique_valid_samples++;
    	results_file << unique_valid_samples << ": " << sample << std::endl;
    }
}

std::string Sampler::model_to_string(const z3::model & m){
    std::string s;
    for (z3::func_decl & v : variables) {
    	s += v.name().str() + ':';
        if (v.range().is_array()) { // array case
            z3::expr e = m.get_const_interp(v);
            Z3_func_decl as_array = Z3_get_as_array_func_decl(c, e);
            if (as_array) {
				z3::func_interp f = m.get_func_interp(to_func_decl(c, as_array));
				std::string num = "[";
				num += std::to_string(f.num_entries());
				s += num + ';';
				std::string def = bv_string(f.else_value(), c);
				s += def + ';';
				for (int j = 0; j < f.num_entries(); ++j) {
					std::string arg = bv_string(f.entry(j).arg(0), c);
					std::string val = bv_string(f.entry(j).value(), c);
					s += arg + ';';
					s += val + ';';
				}
				s += "]";
            } else {
                std::vector<std::string> args;
                std::vector<std::string> values;
                while (e.decl().name().str() == "store") {
                    std::string arg = bv_string(e.arg(1), c);
                    if (std::find(args.begin(), args.end(), arg) != args.end())
                        continue;
                    args.push_back(arg);
                    values.push_back(bv_string(e.arg(2), c));
                    e = e.arg(0);
                }
				std::string num = "[";
				num += std::to_string(args.size());
				s += num + ';';
				std::string def = bv_string(e.arg(0), c);
				s += def + ';';
				for (int j = args.size() - 1; j >= 0; --j) {
					std::string arg = args[j];
					std::string val = values[j];
					s += arg + ';';
					s += val + ';';
				}
				s += "]";
            }
        } else if (v.is_const()) { // BV, Int case
            z3::expr b = m.get_const_interp(v);
            Z3_ast ast = b;
            switch (v.range().sort_kind()) {
            case Z3_BV_SORT:
            {
                if (!ast) {
                    s += bv_string(c.bv_val(0, v.range().bv_size()), c) + ';';
                } else {
                    s += bv_string(b, c) + ';';
                }
                break;
            }
            case Z3_BOOL_SORT:
            {
                if (!ast) {
                    s += std::to_string(false) + ';';
                } else {
                    s += std::to_string(b.bool_value() == Z3_L_TRUE) + ';';
                }
                break;
            }
            case Z3_INT_SORT:
            {
                if (!ast) {
                    s += std::to_string(0) + ";";
                } else {
                	s += b.to_string() + ";";
                }
                break;
            }
            default:
                    std::cout << "Invalid sort\n";
                    exit(1);
            }
        } else { // Uninterpreted function case
            z3::func_interp f = m.get_func_interp(v);
            std::string num = "(";
            num += std::to_string(f.num_entries());
            s += num + ';';
            std::string def = bv_string(f.else_value(), c);
            s += def + ';';
            for (int j = 0; j < f.num_entries(); ++j) {
                for (int k = 0; k < f.entry(j).num_args(); ++k) {
                    std::string arg = bv_string(f.entry(j).arg(k), c);
                    s += arg + ';';
                }
                std::string val = bv_string(f.entry(j).value(), c);
                s += val + ';';
            }
            s += ")";
        }
    }
    return s;
}

void Sampler::set_timer_on(const std::string & category){
	if (is_timer_on.find(category)!=is_timer_on.end() && is_timer_on[category]){ // category was inserted to map and its value was set to true
		std::cout<<"WARNING: starting timer twice for category "<<category<<std::endl;
	}
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	timer_start_times[category] = now;
	is_timer_on[category] = true;
}

void Sampler::accumulate_time(const std::string & category){
	if (is_timer_on.find(category)==is_timer_on.end() || is_timer_on[category]==false){ // timer never went on
			std::cout<<"ERROR: cannot stop timer for category: "<<category<<". Timer was never started."<<std::endl;
			finish();
			exit(1); //TODO add exception handling
	}

	assert(timer_start_times.find(category)!=timer_start_times.end());
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	if (accumulated_times.find(category)==accumulated_times.end()){
		accumulated_times[category] = 0.0;
	}
    accumulated_times[category] += duration(&timer_start_times[category], &now);

    is_timer_on[category] = false;
}
