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
// Authors: Nikita Kavokine, Olivier Parcollet, Nils Wentzell

#include <cmath>
#include <triqs/test_tools/gfs.hpp>
#include <triqs_ctseg/solver_core.hpp>

using triqs::operators::n;
using namespace triqs_ctseg;

TEST(CTSEG, Spin_Spin) {

  mpi::communicator c; // Start the mpi

  double beta         = 10.0;
  double U            = 4.0;
  double mu           = 2.0;
  double epsilon      = 0.3;
  int n_cycles        = 10000;
  int n_warmup_cycles = 1000;
  int length_cycle    = 50;
  int random_seed     = 23488;
  int n_iw            = 5000;
  int n_tau           = 10001;
  double precision    = 1.e-12;

  // Prepare the parameters
  constr_params_t param_constructor;
  solve_params_t param_solve;

  // Construction parameters
  param_constructor.beta          = beta;
  param_constructor.gf_struct     = {{"up", 1}, {"down", 1}};
  param_constructor.n_tau         = n_tau;
  param_constructor.n_tau_bosonic = n_tau;

  // Create solver instance
  solver_core Solver(param_constructor);

  // Solve parameters
  param_solve.h_int             = U * n("up", 0) * n("down", 0);
  param_solve.h_loc0            = -mu * (n("up", 0) + n("down", 0));
  param_solve.n_cycles          = n_cycles;
  param_solve.n_warmup_cycles   = n_warmup_cycles;
  param_solve.length_cycle      = length_cycle;
  param_solve.random_seed       = random_seed;
  param_solve.measure_F_tau     = true;
  param_solve.measure_nn_tau    = true;
  param_solve.measure_nn_static = true;

  // Prepare Delta
  nda::clef::placeholder<0> om_;
  auto Delta_w   = gf<imfreq>({beta, Fermion, n_iw}, {1, 1});
  auto Delta_tau = gf<imtime>({beta, Fermion, param_constructor.n_tau}, {1, 1});
  Delta_w(om_) << 1.0 / (om_ - epsilon) + 1.0 / (om_ + epsilon);
  Delta_tau()           = fourier(Delta_w);
  Solver.Delta_tau()[0] = Delta_tau;
  Solver.Delta_tau()[1] = Delta_tau;

  // Prepare spin-spin interaction
  double l  = 1.0; // electron boson coupling
  double w0 = 1.0; // screening frequency
  auto J0w  = gf<imfreq>({beta, Boson, n_iw}, {1, 1});
  auto D0w  = gf<imfreq>({beta, Boson, n_iw}, {1, 1});
  auto D0t  = gf<imtime>({beta, Boson, param_constructor.n_tau}, {1, 1});
  J0w(om_) << 4 * l * l * w0 / (om_ * om_ - w0 * w0);
  D0w(om_) << l * l * w0 / (om_ * om_ - w0 * w0);
  D0t()                 = fourier(D0w);
  Solver.D0_tau()(0, 0) = D0t;
  Solver.D0_tau()(0, 1) = -D0t;
  Solver.D0_tau()(1, 0) = -D0t;
  Solver.D0_tau()(1, 1) = D0t;
  Solver.Jperp_tau()    = fourier(J0w);

  // Solve
  Solver.solve(param_solve);

  // Save the results
  if (c.rank() == 0) {
    h5::file out_file("spin_spin.out.h5", 'w');
    h5_write(out_file, "G_tau", Solver.results.G_tau);
    h5_write(out_file, "F_tau", Solver.results.F_tau);
    h5_write(out_file, "nn_tau", Solver.results.nn_tau);
    h5_write(out_file, "densities", Solver.results.densities);
  }

  // Compare with reference
  if (c.rank() == 0) {
    h5::file ref_file("spin_spin.ref.h5", 'r');
    block_gf<imtime> G_tau, F_tau;
    block2_gf<imtime> nn_tau;
    std::map<std::string, nda::array<double, 1>> densities;
    h5_read(ref_file, "G_tau", G_tau);
    h5_read(ref_file, "F_tau", F_tau);
    h5_read(ref_file, "nn_tau", nn_tau);
    h5_read(ref_file, "densities", densities);
    EXPECT_ARRAY_NEAR(densities["up"], Solver.results.densities.value()["up"], precision);
    EXPECT_ARRAY_NEAR(densities["down"], Solver.results.densities.value()["down"], precision);
    EXPECT_BLOCK_GF_NEAR(G_tau, Solver.results.G_tau, precision);
    EXPECT_BLOCK_GF_NEAR(F_tau, Solver.results.F_tau.value(), precision);
    EXPECT_BLOCK2_GF_NEAR(nn_tau, Solver.results.nn_tau.value(), precision);
  }
}
MAKE_MAIN;
