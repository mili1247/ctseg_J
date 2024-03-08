#pragma once
#include <vector>
#include "configuration.hpp"
#include "work_data.hpp"

void check_invariant(configuration_t const &config, work_data_t const &wdata);

void check_segments(configuration_t const &config);

void check_dets(configuration_t const &config, work_data_t const &wdata);

void check_jlines(configuration_t const &config);

bool c_in_det(tau_t const &tau, det_t const &D);

bool cdag_in_det(tau_t const &tau, det_t const &D);
