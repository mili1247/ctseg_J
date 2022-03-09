/*******************************************************************************
 * CTSEG: TRIQS hybridization-expansion segment solver
 *
 * Copyright (C) 2013-2018 by T. Ayral, H. Hafermann, P. Delange, M. Ferrero, O.
 *Parcollet
 *
 * CTSEG is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * CTSEG is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * CTSEG. If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/
#include "./measure_gt.hpp"

namespace measures {

  measure_g_f_tau::measure_g_f_tau(params_t const &p, work_data_t const &wdata, configuration_t const &config, results_t &results)
     : wdata{wdata}, config{config}, results{results} {

    beta = p.beta;

    // init g_tau
    auto g_init = block_gf<imtime>{triqs::mesh::imtime{beta, Fermion, p.n_tau}, p.gf_struct};
    g_init()    = 0;

    if (p.measure_gt) g_tau = g_init;
    if (p.measure_ft) f_tau = g_init;
    // otherwise f/g_tau stays empty ...
  }

  // -------------------------------------

  void measure_g_f_tau::accumulate(double s) {
    Z += s;

    for (auto [bl_idx, det] : itertools::enumerate(wdata.dets)) {
      foreach (det, [s, &g = g_tau[bl_idx]](auto const &x, auto const &y, auto const &M) {
        // beta-periodicity is implicit in the argument, just fix the sign properly
        auto val  = (y.first >= x.first ? s : -s) * M;
        auto dtau = double(y.first - x.first);
        g[closest_mesh_pt(dtau)](y.second, x.second) += val;
      })
        ;

    // Fix me F.
    // FIXME : 2 measure : g or f ? Only difference is ???
    // Would simplify !!!!
    }
  }

  // -------------------------------------

  void measure_g_f_tau::collect_results(mpi::communicator const &c) {

    Z = mpi::all_reduce(Z, c);

    auto reduce_g = [this, c](auto &g) {
      g = mpi::all_reduce(g, c);
      g = g / (-this->beta * this->Z * g[0].mesh().delta());
      // Fix the point at zero and beta, for each block
      for (auto &g_bl : g) {
        g_bl[0] *= 2;
        g_bl[g_bl.mesh().size() - 1] *= 2;
      }
      return g;
    };

    // store the result (not reused later, hence we can move it).
    if (g_tau.size() != 0) results.g_tau = reduce_g(g_tau);
    if (f_tau.size() != 0) results.f_tau = reduce_g(f_tau);
  }
} // namespace measures
