#include "split_segment.hpp"

namespace moves {

  double split_segment::attempt() {

    SPDLOG_LOGGER_TRACE("\n =================== ATTEMPT SPLIT ================ \n");

    // ------------ Choice of segment --------------
    // Select color
    color = rng(data.n_color);
    auto &sl       = config.seglists[color];
    SPDLOG_LOGGER_TRACE("Splitting at color {}", color);

    // If color is empty, nothing to split
    if sl.empty() return 0; 

    // Select segment to split 
    proposed_segment_index = rng(sl.size());
    proposed_segment = sl[proposed_segment_index];
    if_full_line = proposed_segment.tau_c == params.beta && proposed_segment.tau_cdag == 0; 
    // Select splitting points (tau_left,tau_right)
    qmc_time_t l = proposed_segment.tau_c - proposed_segment.c_dag; 
    qmc_time_t dt1 = time_point_factory.get_random_pt(l); // FIXME : can this be 0 or l ?? 
    qmc_time_t dt2 = time_point_factory.get_random_pt(l);
    if (dt1 == dt2) return 0; 
    if (dt1 == 0 || dt2 == 0 || dt1 == l || dt2 == l) return 0; 
    if (dt1 > dt2 && !is_full_line) std::swap(dt1,dt2); // If splitting a full line, the order of tau_left and tau_right is not fixed
    tau_left = proposed_segment.tau_c - dt1; 
    tau_right = proposed_segment.tau_c - dt2; 

    SPDLOG_LOGGER_TRACE("Split: adding c at {}, cdag at {}", tau_right, tau_left);

    // ------------  Trace ratio  -------------
        // FIXME : here we will need the K function integral 
    double ln_trace_ratio = 0;
    for (int c : conifg.seglists) {
      if (c != color) ln_trace_ratio -= overlap(segment_t{tau_left,tau_right}, config.seglists[c]);
    }
    double trace_ratio = std::exp(ln_trace_ratio);

    // ------------  Det ratio  ---------------

    // FIXME

    // ------------  Proposition ratio ------------

    double current_number_segments = is_full_line ? 2 : sl.size(); // Account for the two ways of splitting a full line 
    double future_number_intervals = is_full_line ? 1 : sl.size() + 1; 
    double prop_ratio = (future_number_intervals) / (current_number_segments * l * l);

    SPDLOG_LOGGER_TRACE("trace_ratio  = {}, prop_ratio = {}, det_ratio = {}", trace_ratio, prop_ratio, det_ratio);

    return trace_ratio * det_ratio * prop_ratio;
  }

  //--------------------------------------------------

  double split_segment::accept() {

    SPDLOG_LOGGER_TRACE("\n - - - - - ====> ACCEPT - - - - - - - - - - -\n");

    data.dets[color].complete_operation();
    // Split the segment
    auto &sl = config.seglists[color];
    if is_full_line {
      segment_t new_segment = segment_t{tau_right,tau_left};
      sl[proposed_segment_index] = new_segment; 
    }
    else {
      segment_t new_segment_left = segment_t{proposed_segment.tau_c,tau_left}; 
      segment_t new_segment_right = segment_t{tau_right,proposed_segment.tau_cdag}; 
      sl.insert(proposed_segment_index,new_segment_left); 
      sl[proposed_segment_index + 1] = new_segment_right; 
    }

    // FIXME ??? SIGNE ???
    double sign_ratio = 1; 
    return sign_ratio;
  }

  //--------------------------------------------------
  void split_segment::reject() {
    SPDLOG_LOGGER_TRACE("\n - - - - - ====> REJECT - - - - - - - - - - -\n");
    data.dets[color].reject_last_try();
  }
};
} // namespace moves