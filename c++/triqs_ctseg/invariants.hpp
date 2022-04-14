#pragma once
#include <vector>
#include "types.hpp"
#include "configuration.hpp"

void check_invariant(configuration_t const &config, std::vector<det_t> const &dets);

void check_segments(configuration_t const &config);

void check_dets(configuration_t const &config, std::vector<det_t> const &dets);

void check_jlines(configuration_t const &config);