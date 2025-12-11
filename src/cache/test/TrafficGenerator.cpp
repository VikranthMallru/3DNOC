#include "TrafficGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

c_TrafficGenerator::c_TrafficGenerator(const std::string& p_mode, unsigned p_rng_seed, int p_param1, int p_param2) {
    m_mode = p_mode;
    m_rng = std::default_random_engine(p_rng_seed);

    if (m_mode == "uniform") {
        m_uniform_dist = std::make_unique<std::uniform_real_distribution<double>>(0.05, 1.0);
    } else if (m_mode == "beta") {
        m_alpha = (p_param1 > 0.0) ? p_param1 : 2.0;
        m_beta  = (p_param2 > 0.0) ? p_param2 : 5.0;

        //we are using gamma distribution to implement beta distributino
        m_gamma_alpha = std::make_unique<std::gamma_distribution<double>>(m_alpha, 1.0);
        m_gamma_beta  = std::make_unique<std::gamma_distribution<double>>(m_beta, 1.0);
    } else if (m_mode == "poisson") {
        m_lambda = (p_param1 > 0.0) ? p_param1 : 4.0;
        m_poisson_dist = std::make_unique<std::poisson_distribution<int>>(m_lambda);
    }
    else if(m_mode == "");
    else {
        std::cerr << "Error: Unsupported mode '" << m_mode << "'. Using default mode(1 Req/Cycle).\n";   
       // m_mode = "uniform";
       // m_uniform_dist = std::make_unique<std::uniform_real_distribution<double>>(0.05, 1.0); 
    }
}

double c_TrafficGenerator::f_nextProb() {
    if (m_mode == "uniform" && m_uniform_dist) {
        return std::max((*m_uniform_dist)(m_rng), 0.01);  //0.01 is used to avoid very low values which can make #cycles/req very large and impractical
    } else if (m_mode == "beta" && m_gamma_alpha && m_gamma_beta) {
        double l_x = (*m_gamma_alpha)(m_rng);
        double l_y = (*m_gamma_beta)(m_rng);
        if (l_x + l_y == 0.0) return 0.1;
        return std::max(l_x / (l_x + l_y), 0.01);
    } else if (m_mode == "poisson" && m_poisson_dist) {
        int interval = std::max(1, (*m_poisson_dist)(m_rng));
        return 1.0 / interval;
    } else {
        return 1.0; // default 1 req per cycle
    }
}

int c_TrafficGenerator::f_nextWait() {
    if(m_mode == "") return 0;
    return std::round(1.0 / f_nextProb());
}
