#include "./nn_static.hpp"
#include "../logs.hpp"

namespace measures {

  nn_static::nn_static(params_t const &p, work_data_t const &wdata, configuration_t const &config, results_t &results)
     : wdata{wdata}, config{config}, results{results} {

    beta    = p.beta;
    n_color = config.n_color();
    nn      = nda::zeros<double>(n_color, n_color);
  }

  // -------------------------------------

  void nn_static::accumulate(double s) {

    LOG("\n =================== MEASURE <nn>  ================ \n");

    Z += s;

    for (int a = 0; a < n_color; ++a)
      for (int b = 0; b < n_color; ++b) {
        for (auto const &sa : config.seglists[a]) {
          for (auto const &sb : config.seglists[b]) //
            nn(a, b) += s * overlap(sa, sb);
        }
      }
  }
  // -------------------------------------

  void nn_static::collect_results(mpi::communicator const &c) {

    Z  = mpi::all_reduce(Z, c);
    nn = mpi::all_reduce(nn, c);
    nn = nn / Z / beta;

    // store the result (not reused later, hence we can move it).
    results.nn_static = std::move(nn);
  }
} // namespace measures
