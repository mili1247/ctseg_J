#pragma once
#include "../work_data.hpp"
#include "../configuration.hpp"

namespace moves {

  class split_segment {
    work_data_t const &wdata;
    configuration_t &config;
    triqs::mc_tools::random_generator &rng;

    // Internal data
    int color = 0;
    segment_t proposed_segment;
    qmc_time_t tau_left; 
    qmc_time_t tau_right; 
    int proposed_segment_index{};
    bool full_line{}; 
    qmc_time_factory_t time_point_factory = qmc_time_factory_t{wdata.beta};

    public:
    // Constructor
    split_segment(const work_data_t &data_, configuration_t &config_, triqs::mc_tools::random_generator &rng_)
      : wdata(data_), config(config_), rng(rng_){};
    // ------------------
    double attempt();
    double accept();
    void reject();
  };
}; // namespace moves