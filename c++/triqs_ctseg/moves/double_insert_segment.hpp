// Copyright (c) 2022-2024 Simons Foundation
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You may obtain a copy of the License at
//     https://www.gnu.org/licenses/gpl-3.0.txt
//
// Authors: Nikita Kavokine, Hao Lu, Olivier Parcollet, Nils Wentzell

#pragma once
#include "../work_data.hpp"
#include "../configuration.hpp"
#include "../invariants.hpp"

namespace triqs_ctseg::moves {

  class double_insert_segment {
    work_data_t &wdata;
    configuration_t &config;
    triqs::mc_tools::random_generator &rng;

    // Internal data
    std::pair<int, int> colors;
    std::vector<tau_t> window_length = std::vector<tau_t>(2);
    std::vector<segment_t> prop_seg = std::vector<segment_t>(2);
    std::vector<std::pair<int, int>> all_pairs;
    double det_sign;
    bool is_same_block;

    public:
    double_insert_segment(work_data_t &data_, configuration_t &config_, triqs::mc_tools::random_generator &rng_)
       : wdata(data_), config(config_), rng(rng_) {

        // Generate a list of all possible two different color pairs. e.g.
        // if n_color = 4, then outputs (0, 1), (0, 2), (0, 3), (1, 0), 
        // (1, 2), (1, 3), (2, 0), (2, 1), (2, 3), (3, 0), (3, 1), (3, 2)

        for (int i = 0; i < config.n_color(); ++i) {
          for (int j = 0; j < config.n_color(); ++j) {
            if (i != j) all_pairs.emplace_back(i, j);
          }
        }
    };
    // ------------------
    double attempt();
    double accept();
    void reject();
  };

} // namespace triqs_ctseg::moves
