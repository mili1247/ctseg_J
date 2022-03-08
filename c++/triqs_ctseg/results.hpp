#pragma once
#include <optional>
#include "types.hpp"

// One-particle Green's function types
using G_tau_t = block_gf<imtime, matrix_valued>;
using G_iw_t  = block_gf<imfreq, matrix_valued>;

// Gather all the results on the CTQMC
struct results_t {

  /// Single-particle Green's function :math:`G(\tau)` in imaginary time.
  std::optional<G_tau_t> G_tau;

  /// Single-particle Green's function :math:`G(\tau)` in imaginary time.
  std::optional<G_tau_t> chi_tau;

  /// Density per color. FIXME : optional ??
  nda::array<double, 1> densities;
};

/// writes all containers to hdf5 file
void h5_write(h5::group h5group, std::string subgroup_name, results_t const &c);

/// reads all containers to hdf5 file
void h5_read(h5::group h5group, std::string subgroup_name, results_t &c);
