#pragma once
#include "../work_data.hpp"
#include "../configuration.hpp"

namespace moves {
  class regroup_segment_v2 {
    work_data_t &wdata;
    configuration_t &config;
    triqs::mc_tools::random_generator &rng;

    // Internal data
    int color      = 0;
    bool need_flip = false;
    double current_density;
    segment_t prop_seg;
    long prop_seg_idx;
    std::vector<segment_t> sl;
    double det_sign;

    public:
    regroup_segment_v2(work_data_t &data_, configuration_t &config_, triqs::mc_tools::random_generator &rng_)
       : wdata(data_), config(config_), rng(rng_){};
    // ------------------
    double attempt();
    double accept();
    void reject();
  };
}; // namespace moves
