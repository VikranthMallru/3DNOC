#ifndef TRAFFIC_GENERATOR_H
#define TRAFFIC_GENERATOR_H

#include <random>
#include <string>
#include <memory>

class c_TrafficGenerator {
public:
    // Constructor: mode = "uniform", "beta", or "poisson"
    // If mode is:
    //   "beta" then pass alpha and beta in param1 and param2
    //   "poisson" then pass lambda in param1
    //   "uniform" then ignore param1 and param2
    //   default is 1 req/cycle with mode = ""
    // don't change seed value if you want to reproduce the same numbers for debugging.

    c_TrafficGenerator(const std::string& p_mode = "", unsigned p_rng_seed = 123, int p_param1 = 0.0, int p_param2 = 0.0);

    int f_nextWait();  // number of cycles to wait before injecting the next request

private:
    std::string m_mode;
    std::default_random_engine m_rng;

    double m_alpha;
    double m_beta;
    double m_lambda;

    std::unique_ptr<std::uniform_real_distribution<double>> m_uniform_dist;
    std::unique_ptr<std::gamma_distribution<double>> m_gamma_alpha;
    std::unique_ptr<std::gamma_distribution<double>> m_gamma_beta;
    std::unique_ptr<std::poisson_distribution<int>> m_poisson_dist;

    double f_nextProb();  // generate a probability p
};

#endif 
